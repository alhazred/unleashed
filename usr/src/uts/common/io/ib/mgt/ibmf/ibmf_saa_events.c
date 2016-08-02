/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/ib/mgt/ibmf/ibmf_saa_impl.h>
#include <sys/ib/mgt/ibmf/ibmf_saa_utils.h>

extern	saa_state_t	*saa_statep;
extern	int	ibmf_trace_level;

static void
ibmf_saa_informinfo_cb(void *arg, size_t length, char *buffer,
    int status, uint32_t producer_type);

static int
ibmf_saa_send_informinfo(saa_port_t *saa_portp, uint32_t producer_type,
    boolean_t subscribe, boolean_t unseq_unsubscribe);

static void
ibmf_saa_notify_event_client_task(void *args);

static void
ibmf_saa_process_subnet_event(saa_port_t *saa_portp, ib_mad_notice_t *notice);

/*
 * ibmf_saa_subscribe_events:
 * Subscribe or unsubscribe to subnet events for a certain port.
 * ibmf_saa_subscribe_events() will send an InformInfo request for each of the
 * four notice producer types.
 *
 * Subscribes generally occur when the first client for a port opens a session
 * and when a port with registered ibmf_saa clients transitions to active.
 * Subscribes are done as asynchronous, sequenced transactions.
 *
 * ibmf_saa sends unsubscribe requests when the last client for a port
 * unregisters and when an CI_OFFLINE message is received from ibtf (via ibmf).
 * For the first case, the unsubscribe is done as an asynchronous, sequenced
 * transaction.  For the second case, the request is asynchronous, unsequenced.
 * This means that the unsubscribes will not be retried.  Because the port is
 * going away we cannot wait for responses.  Unsubscribes are not required
 * anyway as the SA will remove subscription records from ports it determines to
 * be down.
 *
 * For subscribe requests, clients are notified that the request failed through
 * the event notification mechanism.  For unsubscribe requests,  clients are not
 * notified if the request fails.  Therefore, this function returns void.
 *
 * Input Arguments
 * saa_portp		pointer to port state structure
 * subscribe		B_TRUE if request is a Subscribe, B_FALSE if unsubscribe
 * unseq_unsubscribe	B_TRUE if unsubscribe request should be unsequenced
 * 			(called from CI_OFFLINE event handler)
 * 			B_FALSE if sequenced (wait for response) or for all
 *			subscribe requests
 *
 * Output Arguments
 * none
 *
 * Returns
 * void
 */
void
ibmf_saa_subscribe_events(saa_port_t *saa_portp, boolean_t subscribe,
    boolean_t unseq_unsubscribe)
{
	int				res;
	ibmf_saa_event_details_t	event_details;
	boolean_t			notify_clients = B_FALSE;
	uint8_t				success_mask;

	IBMF_TRACE_0(DPRINT_L4, "ibmf_saa_subscribe_events() enter\n");

	/* subscribes should always be sychronous */
	ASSERT((subscribe == B_FALSE) || (unseq_unsubscribe == B_FALSE));

	/*
	 * reset the arrive and success masks to indicate no responses have come
	 * back; technically only used for subscriptions but reset the values
	 * anyway
	 */
	mutex_enter(&saa_portp->saa_pt_event_sub_mutex);

	success_mask = saa_portp->saa_pt_event_sub_success_mask;

	saa_portp->saa_pt_event_sub_arrive_mask = 0;
	saa_portp->saa_pt_event_sub_success_mask = 0;

	mutex_exit(&saa_portp->saa_pt_event_sub_mutex);

	/*
	 * now subscribe/unsubscribe for each of the notice producer types;
	 * send_informinfo returns 1 on success, 0 on failure.  If the "or" of
	 * all four results is 0 then none of the informinfo's succeed and we
	 * should notify the client.  If it's not 0, then informinfo_cb will be
	 * called at least once, taking care of notifying the clients that there
	 * was a failure.
	 * For each producer type, send the request only if it's a subscribe or
	 * if it's an unsubscribe for a subscribe which succeeded
	 */

	/*
	 * subscribe for all traps generated by the SM;
	 * gid in service/out of service, mgid created/deleted, etc.
	 */
	if ((success_mask & IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_SM) ||
	    (subscribe == B_TRUE))
		res = ibmf_saa_send_informinfo(saa_portp,
		    MAD_INFORMINFO_NODETYPE_SUBNET_MANAGEMENT, subscribe,
		    unseq_unsubscribe);

	/* subscribe for all traps generated by a CA */
	if ((success_mask & IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_CA) ||
	    (subscribe == B_TRUE))
		res |= ibmf_saa_send_informinfo(saa_portp,
		    MAD_INFORMINFO_NODETYPE_CA, subscribe, unseq_unsubscribe);

	/* subscribe for all traps generated by a switch */
	if ((success_mask & IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_SWITCH) ||
	    (subscribe == B_TRUE))
		res |= ibmf_saa_send_informinfo(saa_portp,
		    MAD_INFORMINFO_NODETYPE_SWITCH, subscribe,
		    unseq_unsubscribe);

	/* subscribe for all traps generated by a router */
	if ((success_mask & IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_ROUTER) ||
	    (subscribe == B_TRUE))
		res |= ibmf_saa_send_informinfo(saa_portp,
		    MAD_INFORMINFO_NODETYPE_ROUTER, subscribe,
		    unseq_unsubscribe);

	/* if none of the subscribe requests succeeded notify the clients */
	if ((res == 0) && (subscribe == B_TRUE)) {

		IBMF_TRACE_1(DPRINT_L1, "ibmf_saa_subscribe_events: %s\n",
			     "Could not subscribe for any of the four producer types");

		mutex_enter(&saa_portp->saa_pt_event_sub_mutex);

		/* all events should have "arrived" */
		ASSERT(saa_portp->saa_pt_event_sub_arrive_mask ==
		    IBMF_SAA_PORT_EVENT_SUB_ALL_ARRIVE);

		/* status mask should be 0 since all failed */
		ASSERT(saa_portp->saa_pt_event_sub_success_mask == 0);

		/* notify clients if success mask changed */
		if (saa_portp->saa_pt_event_sub_last_success_mask !=
		    saa_portp->saa_pt_event_sub_success_mask)
			notify_clients = B_TRUE;

		/* update last mask for next set of subscription requests */
		saa_portp->saa_pt_event_sub_last_success_mask =
		    saa_portp->saa_pt_event_sub_arrive_mask = 0;

		mutex_exit(&saa_portp->saa_pt_event_sub_mutex);

		mutex_enter(&saa_portp->saa_pt_mutex);

		/*
		 * Sending the four InformInfos is treated as one port client
		 * reference.  Now that all have returned decrement the
		 * reference count.
		 */
		ASSERT(saa_portp->saa_pt_reference_count > 0);
		saa_portp->saa_pt_reference_count--;

		mutex_exit(&saa_portp->saa_pt_mutex);
	}

	/*
	 * for unsequenced unsubscribes, decrement the reference count here
	 * since no callbacks will ever do it
	 */
	if (unseq_unsubscribe == B_TRUE) {

		mutex_enter(&saa_portp->saa_pt_mutex);

		/*
		 * Sending the four InformInfos is treated as one port client
		 * reference.  Now that all have returned decrement the
		 * reference count.
		 */
		ASSERT(saa_portp->saa_pt_reference_count > 0);
		saa_portp->saa_pt_reference_count--;

		mutex_exit(&saa_portp->saa_pt_mutex);
	}

	if (notify_clients == B_TRUE) {

		bzero(&event_details, sizeof (ibmf_saa_event_details_t));

		ibmf_saa_notify_event_clients(saa_portp, &event_details,
		    IBMF_SAA_EVENT_SUBSCRIBER_STATUS_CHG, NULL);
	}
}

/*
 * ibmf_saa_send_informinfo:
 *
 * Sends an InformInfo request to the SA.  There are two types of request,
 * Subscribes and Unsubscribes.  This function is called from
 * ibmf_saa_subscribe_events.  See that function's comment for usage of
 * subscribe, unseq_unsubscribe booleans.
 *
 * This function generates a standard ibmf_saa transaction and sends using
 * ibmf_saa_impl_send_request().  For asynchronous callbacks, the function
 * ibmf_saa_informinfo_cb() will be called.
 *
 * This function blocks allocating resources, but not waiting for response
 * packets.
 *
 * Input Arguments
 * saa_portp		pointer to port data
 * producer_type	InformInfo producer type to subscribe for
 * subscribe		B_TRUE if subscribe request, B_FALSE if unsubscribe
 * unseq_unsubscribe	B_TRUE if unsubscribe request should be unsequenced
 *			(called from CI_OFFLINE event handler)
 * 			B_FALSE if sequenced (wait for response) or for all
 *			subscribe requests
 *
 * Output Arguments
 * none
 *
 * Returns
 * 1 if the transaction succeeded, 0 if it failed
 */
static int
ibmf_saa_send_informinfo(saa_port_t *saa_portp, uint32_t producer_type,
    boolean_t subscribe, boolean_t unseq_unsubscribe)
{
	ib_mad_informinfo_t	inform_info;
	saa_impl_trans_info_t	*trans_info;
	int			res;
	uint8_t			producer_type_mask;

	IBMF_TRACE_0(DPRINT_L4, "ibmf_saa_send_informinfo() enter\n");

	IBMF_TRACE_4(DPRINT_L3,
		     "ibmf_saa_send_informinfo: %s, producer_type =""%x, subscribe = %x, unseq_unsubscribe = %x\n",
		     "Sending informinfo request",
		     producer_type,
		     subscribe,
		     unseq_unsubscribe);

	bzero(&inform_info, sizeof (ib_mad_informinfo_t));

	/* initialize inform_info packet */
	inform_info.LIDRangeBegin = MAD_INFORMINFO_ALL_ENDPORTS_RANGE;
	inform_info.IsGeneric = MAD_INFORMINFO_FORWARD_GENERIC;

	if (subscribe == B_TRUE)
		inform_info.Subscribe = MAD_INFORMINFO_SUBSCRIBE;
	else {
		inform_info.Subscribe = MAD_INFORMINFO_UNSUBSCRIBE;
		inform_info.QPN = saa_portp->saa_pt_qpn;
	}

	inform_info.Type = MAD_INFORMINFO_TRAP_TYPE_FORWARD_ALL;
	inform_info.TrapNumber_DeviceID =
	    MAD_INFORMINFO_TRAP_NUMBER_FORWARD_ALL;
	inform_info.ProducerType_VendorID = producer_type;

	trans_info = kmem_zalloc(sizeof (saa_impl_trans_info_t), KM_SLEEP);

	/* no specific client associated with this transaction */
	trans_info->si_trans_client_data = NULL;
	trans_info->si_trans_port = saa_portp;
	trans_info->si_trans_method = SA_SUBN_ADM_SET;
	trans_info->si_trans_attr_id = SA_INFORMINFO_ATTRID;
	trans_info->si_trans_component_mask = 0;
	trans_info->si_trans_template = &inform_info;
	trans_info->si_trans_template_length = sizeof (ib_mad_informinfo_t);
	trans_info->si_trans_unseq_unsubscribe = unseq_unsubscribe;

	/*
	 * if this isn't an unsequenced unsubscribe (the only synchronous
	 * request) then set up the callback
	 */
	if (unseq_unsubscribe == B_FALSE) {
		trans_info->si_trans_sub_callback =
		    ibmf_saa_informinfo_cb;
		trans_info->si_trans_callback_arg = saa_portp;

		/*
		 * if this is a subscribe, set the producer type so we can know
		 * which one's failed
		 */
		if (subscribe == B_TRUE) {
			trans_info->si_trans_sub_producer_type = producer_type;
		}
	}

	mutex_enter(&saa_portp->saa_pt_kstat_mutex);

	IBMF_SAA_ADD32_KSTATS(saa_portp, outstanding_requests, 1);
	IBMF_SAA_ADD32_KSTATS(saa_portp, total_requests, 1);

	mutex_exit(&saa_portp->saa_pt_kstat_mutex);

	res = ibmf_saa_impl_send_request(trans_info);
	if (res != IBMF_SUCCESS) {

		IBMF_TRACE_2(DPRINT_L1,
			     "ibmf_saa_send_informinfo: %s, ibmf_status = %d\n",
			     "ibmf_saa_impl_send_request() failed",
			     res);

		res = 0;

	} else {

		IBMF_TRACE_1(DPRINT_L3, "ibmf_saa_send_informinfo: %s\n",
			     "Request sent successfully");

		res = 1;

		/*
		 * if this was an asynchronous transaction (not the unsequenced
		 * unsubscribe case) return here.
		 * The callback will clean up everything.
		 */
		if (unseq_unsubscribe == B_FALSE) {

			goto bail;
		}
	}

	kmem_free(trans_info, sizeof (saa_impl_trans_info_t));

	mutex_enter(&saa_portp->saa_pt_kstat_mutex);

	IBMF_SAA_SUB32_KSTATS(saa_portp, outstanding_requests, 1);
	IBMF_SAA_ADD32_KSTATS(saa_portp, failed_requests, 1);

	mutex_exit(&saa_portp->saa_pt_kstat_mutex);

	/*
	 * if subscribe transaction failed, update status mask
	 * to indicate "response"
	 */
	if ((res == 0) && (subscribe == B_TRUE)) {

		mutex_enter(&saa_portp->saa_pt_event_sub_mutex);

		saa_portp->saa_pt_event_sub_arrive_mask = 0;
		saa_portp->saa_pt_event_sub_success_mask = 0;

		switch (producer_type) {

			case MAD_INFORMINFO_NODETYPE_CA:
				producer_type_mask =
				    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_CA;
			break;
			case MAD_INFORMINFO_NODETYPE_SWITCH:
				producer_type_mask =
				    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_SWITCH;
			break;
			case MAD_INFORMINFO_NODETYPE_ROUTER:
				producer_type_mask =
				    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_ROUTER;
			break;
			case MAD_INFORMINFO_NODETYPE_SUBNET_MANAGEMENT:
				producer_type_mask =
				    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_SM;
			break;
			default:
				IBMF_TRACE_2(DPRINT_L1,
					     "ibmf_saa_send_informinfo: %s, ""producer_type = 0x%x\n",
					     "Unknown producer type",
					     producer_type);

				ASSERT(0);
				producer_type_mask = 0;
			break;
		}

		saa_portp->saa_pt_event_sub_arrive_mask |= producer_type_mask;

		mutex_exit(&saa_portp->saa_pt_event_sub_mutex);
	}

bail:
	IBMF_TRACE_1(DPRINT_L3,
		     "ibmf_saa_send_informinfo() exit: result = 0x%x\n", res);

	return (res);
}

/*
 * ibmf_saa_informinfo_cb:
 *
 * Called when the asynchronous informinfo request receives its response.
 * Checks the status (whether the ibmf_saa was able to subscribe with the SA for
 * events) and updates the status mask for the specific producer.  If all four
 * producer types have arrived then the event clients are notified if there has
 * been a change in the status.
 *
 * Input Arguments
 * arg		user-specified pointer (points to the current port data)
 * length	length of payload returned (should be size of informinfo_rec)
 * buffer	pointer to informinfo response returned (should not be null)
 * status	status of sa access request
 * producer_type for subscriptions, indicates the notice producer type that was
 * 		requested; ignored for unsubscribes
 *
 * Output Arguments
 * none
 *
 * Returns void
 */
static void
ibmf_saa_informinfo_cb(void *arg, size_t length, char *buffer,
    int status, uint32_t producer_type)
{
	saa_port_t			*saa_portp;
	uint8_t				producer_type_mask;
	boolean_t			notify_clients;
	uint8_t				event_status_mask;
	ibmf_saa_event_details_t	event_details;

	IBMF_TRACE_2(DPRINT_L3,
		     "ibmf_saa_informinfo_cb() enter: producer_type ""= 0x%x, status = %d\n",
		     producer_type,
		     status);

	saa_portp = (saa_port_t *)arg;

	notify_clients = B_FALSE;

	/* if producer type is 0 this was an unsubscribe */
	if (producer_type == 0) {

		IBMF_TRACE_1(DPRINT_L3, "ibmf_saa_informinfo_cb(): %s",
			     "handling unsubscribe");

		if (buffer != NULL)
			kmem_free(buffer, length);

		return;
	}

	/* determine which event it was */
	switch (producer_type) {

		case MAD_INFORMINFO_NODETYPE_CA:
			producer_type_mask =
			    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_CA;
		break;
		case MAD_INFORMINFO_NODETYPE_SWITCH:
			producer_type_mask =
			    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_SWITCH;
		break;
		case MAD_INFORMINFO_NODETYPE_ROUTER:
			producer_type_mask =
			    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_ROUTER;
		break;
		case MAD_INFORMINFO_NODETYPE_SUBNET_MANAGEMENT:
			producer_type_mask =
			    IBMF_SAA_EVENT_STATUS_MASK_PRODUCER_SM;
		break;

		default:
			IBMF_TRACE_2(DPRINT_L1,
				     "ibmf_saa_informinfo_cb: %s, ""producer_type = 0x%x\n",
				     "Unknown producer type",
				     producer_type);

			producer_type_mask = 0;
		break;
	}

	mutex_enter(&saa_portp->saa_pt_event_sub_mutex);

	if (saa_portp->saa_pt_event_sub_arrive_mask & producer_type_mask) {

		mutex_exit(&saa_portp->saa_pt_event_sub_mutex);

		IBMF_TRACE_2(DPRINT_L2,
			     "ibmf_saa_informinfo_cb(): %s, prod_type_mask = 0x%x",
			     "Received duplicate response",
			     producer_type_mask);

		if (buffer != NULL)
			kmem_free(buffer, length);

		return;
	}

	saa_portp->saa_pt_event_sub_arrive_mask |= producer_type_mask;

	/* process response */
	if ((status != IBMF_SUCCESS) || (buffer == NULL)) {

		IBMF_TRACE_4(DPRINT_L1,
			     "ibmf_saa_informinfo_cb: %s, status = %d,"" buffer = 0x%p, length = %d\n",
			     "could not get informinfo resp",
			     status,
			     buffer,
			     length);

	} else if (buffer != NULL) {

		kmem_free(buffer, length);
		saa_portp->saa_pt_event_sub_success_mask |= producer_type_mask;
	}

	/* if all four InformInfo responses have arrived */
	if (saa_portp->saa_pt_event_sub_arrive_mask ==
	    IBMF_SAA_PORT_EVENT_SUB_ALL_ARRIVE) {

		IBMF_TRACE_3(DPRINT_L3,
			     "ibmf_saa_informinfo_cb(): %s, success mask = 0x%x,"" last success mask = 0x%x\n",
			     "all informinfo responses have arrived",
			     saa_portp->saa_pt_event_sub_success_mask,
			     saa_portp->saa_pt_event_sub_last_success_mask);

		mutex_enter(&saa_portp->saa_pt_mutex);

		/*
		 * Sending the four InformInfos is treated as one port client
		 * reference.  Now that all have returned decrement the
		 * reference count.
		 */
		ASSERT(saa_portp->saa_pt_reference_count > 0);
		saa_portp->saa_pt_reference_count--;

		mutex_exit(&saa_portp->saa_pt_mutex);

		if (saa_portp->saa_pt_event_sub_last_success_mask !=
		    saa_portp->saa_pt_event_sub_success_mask) {

			IBMF_TRACE_1(DPRINT_L2,
				     "ibmf_saa_informinfo_cb(): %s\n",
				     "success mask different - notifying clients");

			/*
			 * save status mask to give to clients and update last
			 * mask for next set of subscription requests
			 */
			event_status_mask =
			    saa_portp->saa_pt_event_sub_last_success_mask =
			    saa_portp->saa_pt_event_sub_success_mask;

			notify_clients = B_TRUE;
		}
	}

	mutex_exit(&saa_portp->saa_pt_event_sub_mutex);

	if (notify_clients == B_TRUE) {

		bzero(&event_details, sizeof (ibmf_saa_event_details_t));

		event_details.ie_producer_event_status_mask =
		    event_status_mask;

		ibmf_saa_notify_event_clients(saa_portp, &event_details,
		    IBMF_SAA_EVENT_SUBSCRIBER_STATUS_CHG, NULL);
	}
}

/*
 * ibmf_saa_notify_event_client_task
 *
 * Calls the event notification callback for a registered saa client.  Called
 * from ibmf_saa_notify_event_clients() for each client that has registered for
 * events.  ibmf_saa_notify_event_clients() will dispatch this task on the
 * saa_event_taskq so the client's callback can be invoked directly.
 *
 * Input Arguments
 * args			pointer to ibmf_saa_event_taskq_args_t
 *			this function will free memory associated with args
 *
 * Output Arguments
 * none
 *
 * Returns
 * void
 */
static void
ibmf_saa_notify_event_client_task(void *args)
{
	ibmf_saa_event_taskq_args_t	*event_taskq_args;
	saa_client_data_t		*client;

	IBMF_TRACE_0(DPRINT_L3, "ibmf_saa_notify_event_client_task() enter\n");

	event_taskq_args = (ibmf_saa_event_taskq_args_t *)args;

	client = event_taskq_args->et_client;

	/* call client's callback (client pointer is ibmf_saa_handle) */
	(event_taskq_args->et_callback)((ibmf_saa_handle_t)client,
	    event_taskq_args->et_subnet_event,
	    event_taskq_args->et_event_details,
	    event_taskq_args->et_callback_arg);

	kmem_free(event_taskq_args->et_event_details,
	    sizeof (ibmf_saa_event_details_t));

	kmem_free(event_taskq_args, sizeof (ibmf_saa_event_taskq_args_t));

	/* decrement the callback count and signal a waiting client */
	mutex_enter(&client->saa_client_mutex);

	client->saa_client_event_cb_num_active--;

	if (client->saa_client_event_cb_num_active == 0) {

		cv_signal(&client->saa_client_event_cb_cv);

	}

	mutex_exit(&client->saa_client_mutex);

	IBMF_TRACE_0(DPRINT_L3, "ibmf_saa_notify_event_client_task() exit\n");
}

/*
 * ibmf_saa_process_subnet_event:
 *
 * Called when the ibmf_saa is notified of a forwarded notice.  Converts the
 * notice into an ibmf_saa_event_details structure and calls
 * ibmf_saa_notify_event_clients() which will notify each interested client.
 *
 * Input Arguments
 * saa_portp		pointer to saa_port data
 * notice		notice that was forwarded from SA
 *
 * Output Arguments
 * none
 *
 * Returns
 * void
 */
static void
ibmf_saa_process_subnet_event(saa_port_t *saa_portp, ib_mad_notice_t *notice)
{
	ibmf_saa_event_details_t	event_details;
	sm_trap_64_t			trap_data_details;
	sm_trap_144_t			cap_mask_trap_data_details;
	sm_trap_145_t			sys_img_trap_data_details;
	ibmf_saa_subnet_event_t		subnet_event;

	IBMF_TRACE_1(DPRINT_L3,
		     "ibmf_saa_process_subnet_event() enter: ""trap_number = 0x%x\n",
		     notice->TrapNumber_DeviceID);

	bzero(&event_details, sizeof (ibmf_saa_event_details_t));

	/*
	 * fill in the appropriate fields of event_details depending on
	 * the trap number
	 */
	switch (notice->TrapNumber_DeviceID) {

		case SM_GID_IN_SERVICE_TRAP:

			ibmf_saa_gid_trap_parse_buffer(notice->DataDetails,
			    &trap_data_details);

			event_details.ie_gid = trap_data_details.GIDADDR;

			subnet_event = IBMF_SAA_EVENT_GID_AVAILABLE;
		break;


		case SM_GID_OUT_OF_SERVICE_TRAP:

			ibmf_saa_gid_trap_parse_buffer(notice->DataDetails,
			    &trap_data_details);

			event_details.ie_gid = trap_data_details.GIDADDR;

			subnet_event = IBMF_SAA_EVENT_GID_UNAVAILABLE;
		break;

		case SM_MGID_CREATED_TRAP:

			ibmf_saa_gid_trap_parse_buffer(notice->DataDetails,
			    &trap_data_details);

			event_details.ie_gid = trap_data_details.GIDADDR;

			subnet_event = IBMF_SAA_EVENT_MCG_CREATED;
		break;

		case SM_MGID_DESTROYED_TRAP:

			ibmf_saa_gid_trap_parse_buffer(notice->DataDetails,
			    &trap_data_details);

			event_details.ie_gid = trap_data_details.GIDADDR;

			subnet_event = IBMF_SAA_EVENT_MCG_DELETED;
		break;

		case SM_CAP_MASK_CHANGED_TRAP:

			ibmf_saa_capmask_chg_trap_parse_buffer(
			    notice->DataDetails, &cap_mask_trap_data_details);

			event_details.ie_lid =
			    cap_mask_trap_data_details.LIDADDR;
			event_details.ie_capability_mask =
			    cap_mask_trap_data_details.CAPABILITYMASK;

			subnet_event = IBMF_SAA_EVENT_CAP_MASK_CHG;
		break;

		case SM_SYS_IMG_GUID_CHANGED_TRAP:

			ibmf_saa_sysimg_guid_chg_trap_parse_buffer(
			    notice->DataDetails, &sys_img_trap_data_details);

			event_details.ie_lid =
			    sys_img_trap_data_details.LIDADDR;
			event_details.ie_sysimg_guid =
			    sys_img_trap_data_details.SYSTEMIMAGEGUID;

			subnet_event = IBMF_SAA_EVENT_SYS_IMG_GUID_CHG;
		break;

		default:
			/*
			 * do nothing if it's not one of the traps we care about
			 */
			IBMF_TRACE_1(DPRINT_L3,
				     "ibmf_saa_process_subnet_event() exit: %s\n",
				     "not one of the six ibmf_saa subnet events");

			return;
	}

	ibmf_saa_notify_event_clients(saa_portp, &event_details, subnet_event,
	    NULL);
}

/*
 * ibmf_saa_notify_event_clients:
 *
 * Called when a trap for one of the six saa subnet events arrives or there is a
 * change in the status of event subscriptions.  Searches the list of clients
 * with callbacks and dispatches a taskq thread to notify the client that the
 * event occured.
 *
 * If some subscription request fails and a subsequent client registers for
 * events that client needs to know that it may not receive all events.  To
 * facilitate this, notify_event_clients() takes an optional parameter which
 * specifies a specific client.  If registering_client is non-NULL only this
 * client is notified.  If the parameter is NULL, all clients in the list are
 * notified.
 *
 * Input Arguments
 * saa_portp		pointer to saa_port data
 * event_details	pointer to ibmf_saa_event_details_t for this event
 * subnet_event		type of event that occured
 * registering_client	pointer to client_data_t if notification should go to a
 *			specific client; NULL if notification should go to all
 *			clients which subscribed for events
 *
 * Output Arguments
 * none
 *
 * Returns
 * none
 */
void
ibmf_saa_notify_event_clients(saa_port_t *saa_portp,
    ibmf_saa_event_details_t *event_details,
    ibmf_saa_subnet_event_t subnet_event, saa_client_data_t *registering_client)
{
	saa_client_data_t		*client;
	ibmf_saa_event_taskq_args_t	*event_taskq_args;
	int				status;

	IBMF_TRACE_0(DPRINT_L4, "ibmf_saa_notify_event_clients() enter\n");

	mutex_enter(&saa_portp->saa_pt_event_sub_mutex);

	if (registering_client != NULL)
		client = registering_client;
	else
		client = saa_portp->saa_pt_event_sub_client_list;

	while (client != NULL) {

		_NOTE(NOW_INVISIBLE_TO_OTHER_THREADS(*event_taskq_args))

		event_taskq_args = kmem_zalloc(
		    sizeof (ibmf_saa_event_taskq_args_t), KM_NOSLEEP);
		if (event_taskq_args == NULL) {

			IBMF_TRACE_2(DPRINT_L1,
				     "ibmf_saa_notify_event_clients: %s, client = ""0x%x\n",
				     "could not allocate memory for taskq args",
				     client);

			/*
			 * if a particular client was not specified continue
			 * processing the client list
			 */
			if (registering_client == NULL)
				client = client->next;
			else
				client = NULL;

			continue;
		}

		/*
		 * each task needs its own pointer, the task will free
		 * up this memory
		 */
		event_taskq_args->et_event_details = kmem_zalloc(
		    sizeof (ibmf_saa_event_details_t), KM_NOSLEEP);
		if (event_taskq_args->et_event_details == NULL) {

			IBMF_TRACE_2(DPRINT_L1,
				     "ibmf_saa_notify_event_clients: %s, client = ""0x%x\n",
				     "could not allocate memory for taskq event details",
				     client);

			kmem_free(event_taskq_args,
			    sizeof (ibmf_saa_event_taskq_args_t));

			/*
			 * if a particular client was not specified continue
			 * processing the client list
			 */
			client =
			    (registering_client == NULL) ? client->next: NULL;

			continue;
		}

		mutex_enter(&client->saa_client_mutex);

		/*
		 * don't generate callbacks if client is not active
		 * (it's probably closing the session)
		 */
		if (client->saa_client_state != SAA_CLIENT_STATE_ACTIVE) {

			IBMF_TRACE_3(DPRINT_L2,
				     "ibmf_saa_notify_event_clients: %s, client = ""0x%x, state = 0x%x\n",
				     "client state not active",
				     client,
				     client->saa_client_state);

			mutex_exit(&client->saa_client_mutex);

			kmem_free(event_taskq_args->et_event_details,
			    sizeof (ibmf_saa_event_details_t));

			kmem_free(event_taskq_args,
			    sizeof (ibmf_saa_event_taskq_args_t));

			/*
			 * if a particular client was not specified continue
			 * processing the client list
			 */
			client =
			    (registering_client == NULL) ? client->next: NULL;

			continue;
		}

		/*
		 * increment the callback count so the client cannot close the
		 * session while callbacks are active
		 */
		client->saa_client_event_cb_num_active++;

		mutex_exit(&client->saa_client_mutex);

		event_taskq_args->et_client = client;
		event_taskq_args->et_subnet_event = subnet_event;

		bcopy(event_details, event_taskq_args->et_event_details,
		    sizeof (ibmf_saa_event_details_t));

		event_taskq_args->et_callback = client->saa_client_event_cb;
		event_taskq_args->et_callback_arg =
		    client->saa_client_event_cb_arg;

		_NOTE(NOW_VISIBLE_TO_OTHER_THREADS(*event_taskq_args))

		/* dispatch taskq thread to notify client */
		status = taskq_dispatch(saa_statep->saa_event_taskq,
		    ibmf_saa_notify_event_client_task, event_taskq_args,
		    KM_NOSLEEP);
		if (status == 0) {

			IBMF_TRACE_2(DPRINT_L1,
				     "ibmf_saa_notify_event_clients: %s, client = ""0x%x\n",
				     "Could not dispatch event taskq",
				     client);

			kmem_free(event_taskq_args->et_event_details,
			    sizeof (ibmf_saa_event_details_t));

			kmem_free(event_taskq_args,
			    sizeof (ibmf_saa_event_taskq_args_t));

			/*
			 * decrement the callback count and signal a waiting
			 * client
			 */
			mutex_enter(&client->saa_client_mutex);

			client->saa_client_event_cb_num_active--;

			if (client->saa_client_event_cb_num_active == 0) {

				cv_signal(&client->saa_client_event_cb_cv);

			}

			mutex_exit(&client->saa_client_mutex);

		} else {

			IBMF_TRACE_2(DPRINT_L3,
				     "ibmf_saa_notify_event_clients: %s, client = ""0x%x\n",
				     "Dispatched task to notify client",
				     client);
		}


		/*
		 * if a particular client was not specified continue processing
		 * the client list
		 */
		client = (registering_client == NULL) ? client->next: NULL;
	}

	mutex_exit(&saa_portp->saa_pt_event_sub_mutex);
}

/*
 * ibmf_saa_report_cb:
 *
 * Called when a forwarded notice Report is received by ibmf_saa from the SA.
 * Converts the Report into an ib_mad_notice_t and calls
 * ibmf_saa_notify_event_clients() which will notify each subscribed ibmf_saa
 * client.  Also sends a response to the report to acknowledge to the SA that
 * this port is still up.
 *
 * This is the registered async callback with ibmf.  Only Reports should come
 * through this interface as all other transactions with ibmf_saa are sequenced
 * (ibmf_saa makes the initial request).
 *
 * This function cannot block since it is called from an ibmf callback.
 *
 * Input Arguments
 * ibmf_handle 			ibmf handle
 * msgp				pointer to ibmf_msg_t
 * args				pointer to saa_port data
 *
 * Output Arguments
 * none
 *
 * Returns
 * none
 */
void
ibmf_saa_report_cb(ibmf_handle_t ibmf_handle, ibmf_msg_t *msgp,
    void *args)
{
	ib_mad_hdr_t		*req_mad_hdr, *resp_mad_hdr;
	saa_port_t		*saa_portp, *saa_port_list_entry;
	ibmf_retrans_t		ibmf_retrans;
	int			ibmf_status;
	ib_mad_notice_t		*notice_report;
	saa_impl_trans_info_t	*trans_info;
	boolean_t		port_valid;
	uint16_t		mad_status;
	uint16_t		attr_id;
	boolean_t		response_sent = B_FALSE;
	size_t			length;
	int			status;

	IBMF_TRACE_0(DPRINT_L4, "ibmf_saa_report_cb() enter\n");

	_NOTE(ASSUMING_PROTECTED(*msgp))

	saa_portp = (saa_port_t *)args;

	port_valid = B_FALSE;

	/* check whether this portp is still valid */
	mutex_enter(&saa_statep->saa_port_list_mutex);

	saa_port_list_entry = saa_statep->saa_port_list;
	while (saa_port_list_entry != NULL) {

		if (saa_port_list_entry == saa_portp) {

			port_valid = ibmf_saa_is_valid(saa_portp, B_FALSE);

			break;
		}
		saa_port_list_entry = saa_port_list_entry->next;
	}

	mutex_exit(&saa_statep->saa_port_list_mutex);

	if (port_valid == B_FALSE) {

		IBMF_TRACE_2(DPRINT_L2,
			     "ibmf_saa_report_cb: %s, saa_port = 0x%p\n",
			     "port no longer valid",
			     saa_portp);

		goto bail;
	}

	req_mad_hdr = msgp->im_msgbufs_recv.im_bufs_mad_hdr;

	/* drop packet if status is bad */
	if ((msgp->im_msg_status != IBMF_SUCCESS) ||
	    (req_mad_hdr == NULL) ||
	    ((mad_status = b2h16(req_mad_hdr->Status)) != SA_STATUS_NO_ERROR)) {

		IBMF_TRACE_4(DPRINT_L1,
			     "ibmf_saa_report_cb: %s, msg_status = 0x%x,"" req_mad_hdr = 0x%p, mad_status = 0x%x\n",
			     "Bad ibmf status",
			     msgp->im_msg_status,
			     req_mad_hdr,
			     (req_mad_hdr == NULL ? 0 : mad_status));

		goto bail;
	}

	/* drop packet if class version is not correct */
	if (req_mad_hdr->ClassVersion != SAA_MAD_CLASS_VERSION) {

		IBMF_TRACE_3(DPRINT_L1,
			     "ibmf_saa_report_cb: %s, msg_class_ver = 0x%x,"" ibmf_saa_class_ver = 0x%x\n",
			     "Bad class version",
			     req_mad_hdr->ClassVersion,
			     SAA_MAD_CLASS_VERSION);

		goto bail;
	}


	/*
	 * only care about notice reports(); should not get any other type
	 * of method or attribute
	 */
	if (((attr_id = b2h16(req_mad_hdr->AttributeID)) != SA_NOTICE_ATTRID) ||
	    (req_mad_hdr->R_Method != SA_SUBN_ADM_REPORT)) {

		IBMF_TRACE_3(DPRINT_L2,
			     "ibmf_saa_report_cb: %s, attr_id = 0x%x, ""method = 0x%x\n",
			     "Unsolicited message not notice report",
			     attr_id,
			     req_mad_hdr->R_Method);

		goto bail;
	}

	/*
	 * unpack the data into a ib_mad_notice_t; the data details are left
	 * as packed data and will be unpacked by process_subnet_event()
	 * is_get_resp parameter is set to B_TRUE since cl_data_len will
	 * probably be set to 200 bytes by ibmf (it's not an RMPP trans)
	 */
	status = ibmf_saa_utils_unpack_payload(
	    msgp->im_msgbufs_recv.im_bufs_cl_data,
	    msgp->im_msgbufs_recv.im_bufs_cl_data_len, SA_NOTICE_ATTRID,
	    (void **)&notice_report, &length, 0, B_TRUE, KM_NOSLEEP);
	if (status != IBMF_SUCCESS) {

		IBMF_TRACE_2(DPRINT_L2,
			     "ibmf_saa_report_cb: %s, status = %d",
			     "Could not unpack data",
			     status);

		goto bail;
	}

	ASSERT(length == sizeof (ib_mad_notice_t));

	ibmf_saa_process_subnet_event(saa_portp, notice_report);

	kmem_free(notice_report, length);

	_NOTE(NOW_INVISIBLE_TO_OTHER_THREADS(*resp_mad_hdr))

	/* send ReportResp */
	resp_mad_hdr = kmem_zalloc(sizeof (ib_mad_hdr_t), KM_SLEEP);

	bcopy(req_mad_hdr, resp_mad_hdr, sizeof (ib_mad_hdr_t));

	resp_mad_hdr->R_Method = SA_SUBN_ADM_REPORT_RESP;

	msgp->im_msgbufs_send.im_bufs_mad_hdr = resp_mad_hdr;
	msgp->im_msgbufs_send.im_bufs_cl_hdr = kmem_zalloc(
	    msgp->im_msgbufs_recv.im_bufs_cl_hdr_len, KM_SLEEP);
	msgp->im_msgbufs_send.im_bufs_cl_hdr_len =
	    msgp->im_msgbufs_recv.im_bufs_cl_hdr_len;

	/* only headers are needed */
	msgp->im_msgbufs_send.im_bufs_cl_data = NULL;
	msgp->im_msgbufs_send.im_bufs_cl_data_len = 0;

	/*
	 * report_cb cannot block because it's in the context of an ibmf
	 * callback.  So the response needs to be sent asynchronously.
	 * ibmf_saa_async_cb is an appropriate callback to use for the response.
	 * Set up a trans_info structure as saa_async_cb expects.  But don't use
	 * ibmf_saa_impl_send_request() to send the response since that function
	 * does unncessary steps in this case (like allocating a new ibmf msg).
	 * Only the si_trans_port field needs to be filled in.
	 */
	trans_info = kmem_zalloc(sizeof (saa_impl_trans_info_t), KM_NOSLEEP);
	if (trans_info == NULL) {

		IBMF_TRACE_1(DPRINT_L1, "ibmf_saa_report_cb: %s",
			     "could not allocate trans_info structure");

		goto bail;
	}

	trans_info->si_trans_port = saa_portp;

	mutex_enter(&saa_portp->saa_pt_mutex);

	bcopy(&saa_portp->saa_pt_ibmf_retrans, &ibmf_retrans,
	    sizeof (ibmf_retrans_t));

	saa_portp->saa_pt_num_outstanding_trans++;

	mutex_exit(&saa_portp->saa_pt_mutex);

	ASSERT(ibmf_handle == saa_portp->saa_pt_ibmf_handle);

	ibmf_status = ibmf_msg_transport(ibmf_handle,
	    saa_portp->saa_pt_qp_handle, msgp, &ibmf_retrans, ibmf_saa_async_cb,
	    trans_info, 0);
	if (ibmf_status != IBMF_SUCCESS) {

		IBMF_TRACE_2(DPRINT_L1,
			     "ibmf_saa_report_cb: %s, msg_status = 0x%x\n",
			     "Could not send report response",
			     ibmf_status);

		mutex_enter(&saa_portp->saa_pt_mutex);

		ASSERT(saa_portp->saa_pt_num_outstanding_trans > 0);
		saa_portp->saa_pt_num_outstanding_trans--;

		mutex_exit(&saa_portp->saa_pt_mutex);

		kmem_free(trans_info, sizeof (saa_impl_trans_info_t));

	} else {

		IBMF_TRACE_1(DPRINT_L3, "ibmf_saa_report_cb: %s\n",
			     "Asynchronous Report response sent");

		response_sent = B_TRUE;
	}

bail:
	if (response_sent == B_FALSE) {
		ibmf_status = ibmf_free_msg(ibmf_handle, &msgp);
		ASSERT(ibmf_status == IBMF_SUCCESS);
	}
}

/*
 * ibmf_saa_add_event_subscriber:
 *
 * Adds an interested client to the list of subscribers for events for a port.
 * If it's the first client, generates the subscription requests.
 * This function must only be called if event_args is not null
 *
 * Input Arguments
 *
 * client		pointer to client data (client->saa_port should be set)
 * event_args		pointer to event_args passed in from client (non-NULL)
 *
 * Output Arguments
 * none
 *
 * Returns
 * void
 */
void
ibmf_saa_add_event_subscriber(saa_client_data_t *client,
    ibmf_saa_subnet_event_args_t *event_args)
{
	saa_port_t			*saa_portp;
	boolean_t			first_client;
	uint8_t				producer_status_mask;
	ibmf_saa_event_details_t	event_details;

	IBMF_TRACE_0(DPRINT_L4, "ibmf_saa_add_event_subscriber() enter\n");

	/* event_args should be checked before calling this function */
	ASSERT(event_args != NULL);

	/* don't add client if no callback function is specified */
	if (event_args->is_event_callback == NULL)
		return;

	saa_portp = client->saa_client_port;

	client->saa_client_event_cb = event_args->is_event_callback;
	client->saa_client_event_cb_arg = event_args->is_event_callback_arg;

	/*
	 * insert this client onto the list; this list is used when a
	 * Report arrives to call each client's callback
	 */
	mutex_enter(&saa_portp->saa_pt_event_sub_mutex);

	IBMF_TRACE_2(DPRINT_L3,
		     "ibmf_saa_add_event_subscriber: %s, client = 0x%x\n",
		     "Adding client to event subscriber list",
		     client);

	if (saa_portp->saa_pt_event_sub_client_list == NULL)
		first_client = B_TRUE;
	else {
		first_client = B_FALSE;
		producer_status_mask =
		    saa_portp->saa_pt_event_sub_last_success_mask;
	}

	client->next = saa_portp->saa_pt_event_sub_client_list;
	saa_portp->saa_pt_event_sub_client_list = client;

	mutex_exit(&saa_portp->saa_pt_event_sub_mutex);

	if (first_client == B_TRUE) {

		/*
		 * increment the reference count by one to account for
		 * the subscription requests.  All four InformInfo's are
		 * sent as one port client reference.
		 */
		mutex_enter(&saa_portp->saa_pt_mutex);

		saa_portp->saa_pt_reference_count++;

		mutex_exit(&saa_portp->saa_pt_mutex);

		/* subscribe for subnet events */
		ibmf_saa_subscribe_events(saa_portp, B_TRUE, B_FALSE);

	} else if (producer_status_mask != IBMF_SAA_PORT_EVENT_SUB_ALL_ARRIVE) {

		/*
		 * if this is not the first client and the producer status mask
		 * is not all success, generate a callback to indicate to the
		 * client that not all events will be forwarded
		 */
		bzero(&event_details, sizeof (ibmf_saa_event_details_t));

		event_details.ie_producer_event_status_mask =
		    producer_status_mask;

		ibmf_saa_notify_event_clients(saa_portp, &event_details,
		    IBMF_SAA_EVENT_SUBSCRIBER_STATUS_CHG, client);
	}
}
