namespace hfsm2 {
namespace detail {

////////////////////////////////////////////////////////////////////////////////

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRegister(StateRegistry& stateRegistry,
										const Parent parent)
{
	stateRegistry.orthoParents[ORTHO_INDEX] = parent;
	stateRegistry.orthoUnits[ORTHO_INDEX] = Units{ORTHO_UNIT, WIDTH};

	_headState.deepRegister(stateRegistry, parent);
	_subStates.wideRegister(stateRegistry, ORTHO_ID);
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
bool
O_<TN_, TA_, TH_, TS_...>::deepForwardEntryGuard(GuardControl& control) {
	const ProngConstBits requested = orthoRequested(static_cast<const GuardControl&>(control));

	ScopedRegion region{control, REGION_ID, HEAD_ID, REGION_SIZE};

	if (requested)
		return _subStates.wideForwardEntryGuard(control, requested);
	else
		return _subStates.wideForwardEntryGuard(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
bool
O_<TN_, TA_, TH_, TS_...>::deepEntryGuard(GuardControl& control) {
	ScopedRegion region{control, REGION_ID, HEAD_ID, REGION_SIZE};

	return _headState.deepEntryGuard(control) ||
		   _subStates.wideEntryGuard(control);
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepEnter(PlanControl& control) {
	ProngBits requested = orthoRequested(control);
	requested.clear();

	ScopedRegion region{control, REGION_ID, HEAD_ID, REGION_SIZE};

	_headState.deepEnter(control);
	_subStates.wideEnter(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepReenter(PlanControl& control) {
	ProngBits requested = orthoRequested(control);
	requested.clear();

	ScopedRegion region{control, REGION_ID, HEAD_ID, REGION_SIZE};

	_headState.deepReenter(control);
	_subStates.wideReenter(control);
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
Status
O_<TN_, TA_, TH_, TS_...>::deepUpdate(FullControl& control) {
	ScopedRegion outer{control, REGION_ID, HEAD_ID, REGION_SIZE};

	if (const auto headStatus = _headState.deepUpdate(control)) {
		ControlLock lock{control};
		_subStates.wideUpdate(control);

		return headStatus;
	} else {
		const Status subStatus = _subStates.wideUpdate(control);

		if (subStatus.outerTransition)
			return subStatus;

		ScopedRegion inner{control, REGION_ID, HEAD_ID, REGION_SIZE};

		return subStatus && control.planData().planExists.template get<REGION_ID>() ?
			control.updatePlan(_headState, subStatus) : subStatus;
	}
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
template <typename TEvent>
Status
O_<TN_, TA_, TH_, TS_...>::deepReact(FullControl& control,
									 const TEvent& event)
{
	ScopedRegion outer{control, REGION_ID, HEAD_ID, REGION_SIZE};

	if (const auto headStatus = _headState.deepReact(control, event)) {
		ControlLock lock{control};
		_subStates.wideReact(control, event);

		return headStatus;
	} else {
		const Status subStatus = _subStates.wideReact(control, event);

		if (subStatus.outerTransition)
			return subStatus;

		ScopedRegion inner{control, REGION_ID, HEAD_ID, REGION_SIZE};

		return subStatus && control.planData().planExists.template get<REGION_ID>() ?
			control.updatePlan(_headState, subStatus) : subStatus;
	}
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
bool
O_<TN_, TA_, TH_, TS_...>::deepForwardExitGuard(GuardControl& control) {
	const ProngConstBits requested = orthoRequested(static_cast<const GuardControl&>(control));

	ScopedRegion region{control, REGION_ID, HEAD_ID, REGION_SIZE};

	if (requested)
		return _subStates.wideForwardExitGuard(control, requested);
	else
		return _subStates.wideForwardExitGuard(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
bool
O_<TN_, TA_, TH_, TS_...>::deepExitGuard(GuardControl& control) {
	ScopedRegion region{control, REGION_ID, HEAD_ID, REGION_SIZE};

	return _headState.deepExitGuard(control) ||
		   _subStates.wideExitGuard(control);
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepExit(PlanControl& control) {
	_subStates.wideExit(control);
	_headState.deepExit(control);
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepForwardActive(Control& control,
											 const RequestType request)
{
	HFSM_ASSERT(control._stateRegistry.isActive(HEAD_ID));

	const ProngConstBits requested = orthoRequested(static_cast<const Control&>(control));
	HFSM_ASSERT(!!requested);

	_subStates.wideForwardActive(control, request, requested);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepForwardRequest(Control& control,
											  const RequestType request)
{
	const ProngConstBits requested = orthoRequested(static_cast<const Control&>(control));

	if (requested)
		_subStates.wideForwardRequest(control, request, requested);
	else
		deepRequest					 (control, request);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequest(Control& control,
									   const RequestType request)
{
	switch (request) {
	case Request::REMAIN:
		deepRequestRemain (control._stateRegistry);
		break;

	case Request::CHANGE:
		deepRequestChange (control);
		break;

	case Request::RESTART:
		deepRequestRestart(control._stateRegistry);
		break;

	case Request::RESUME:
		deepRequestResume (control._stateRegistry);
		break;

	case Request::UTILIZE:
		deepRequestUtilize(control);
		break;

	case Request::RANDOMIZE:
		deepRequestRandomize(control);
		break;

	default:
		HFSM_BREAK();
	}
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequestChange(Control& control) {
	_subStates.wideRequestChange(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequestRemain(StateRegistry& stateRegistry) {
	_subStates.wideRequestRemain(stateRegistry);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequestRestart(StateRegistry& stateRegistry) {
	_subStates.wideRequestRestart(stateRegistry);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequestResume(StateRegistry& stateRegistry) {
	_subStates.wideRequestResume(stateRegistry);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequestUtilize(Control& control) {
	_subStates.wideRequestUtilize(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepRequestRandomize(Control& control) {
	_subStates.wideRequestRandomize(control);
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
typename TA_::UP
O_<TN_, TA_, TH_, TS_...>::deepReportChange(Control& control) {
	const UP	  h = _headState.deepReportChange(control);
	const Utility s = _subStates.wideReportChange(control);

	const Utility sub = s / WIDTH;

	HFSM_LOG_UTILITY_RESOLUTION(control.context(), HEAD_ID, INVALID_STATE_ID, sub);

	return {
		h.utility * sub,
		h.prong
	};
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
typename TA_::UP
O_<TN_, TA_, TH_, TS_...>::deepReportUtilize(Control& control) {
	const UP	  h = _headState.deepReportUtilize(control);
	const Utility s = _subStates.wideReportUtilize(control);

	const Utility sub = s / WIDTH;

	HFSM_LOG_UTILITY_RESOLUTION(control.context(), HEAD_ID, INVALID_STATE_ID, sub);

	return {
		h.utility * sub,
		h.prong
	};
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
typename TA_::Rank
O_<TN_, TA_, TH_, TS_...>::deepReportRank(Control& control) {
	return _headState.wrapRank(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
typename TA_::Utility
O_<TN_, TA_, TH_, TS_...>::deepReportRandomize(Control& control) {
	const Utility h = _headState.wrapUtility(control);
	const Utility s = _subStates.wideReportRandomize(control);

	const Utility sub = s / WIDTH;

	HFSM_LOG_RANDOM_RESOLUTION(control.context(), HEAD_ID, INVALID_STATE_ID, sub);

	return h * sub;
}

//------------------------------------------------------------------------------

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepEnterRequested(PlanControl& control) {
	_headState.deepEnter		 (control);
	_subStates.wideEnterRequested(control);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepChangeToRequested(PlanControl& control) {
	_subStates.wideChangeToRequested(control);
}

//------------------------------------------------------------------------------

#ifdef HFSM_ENABLE_STRUCTURE_REPORT

template <typename TN_, typename TA_, typename TH_, typename... TS_>
void
O_<TN_, TA_, TH_, TS_...>::deepGetNames(const LongIndex parent,
										const RegionType region,
										const ShortIndex depth,
										StructureStateInfos& _stateInfos) const
{
	_headState.deepGetNames(parent, region,			 depth,		_stateInfos);
	_subStates.wideGetNames(_stateInfos.count() - 1, depth + 1, _stateInfos);
}

#endif

////////////////////////////////////////////////////////////////////////////////

}
}
