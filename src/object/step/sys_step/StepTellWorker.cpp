/*******************************************************************************
 * Project:  Nebula
 * @file     StepTellWorker.cpp
 * @brief 
 * @author   Bwar
 * @date:    2016年8月16日
 * @note
 * Modify history:
 ******************************************************************************/
#include "StepTellWorker.hpp"

namespace neb
{

StepTellWorker::StepTellWorker(const tagChannelContext& stCtx)
    : m_stCtx(stCtx)
{
}

StepTellWorker::~StepTellWorker()
{
}

E_CMD_STATUS StepTellWorker::Emit(
        int iErrno,
        const std::string& strErrMsg,
        void* data)
{
    MsgBody oOutMsgBody;
    TargetWorker oTargetWorker;
    oTargetWorker.set_worker_identify(GetWorkerIdentify());
    oTargetWorker.set_node_type(GetNodeType());
    oOutMsgBody.mutable_rsp_result()->set_code(0);
    oOutMsgBody.mutable_rsp_result()->set_msg("OK");
    oOutMsgBody.set_data(oTargetWorker.SerializeAsString());
    Step::SendTo(m_stCtx, CMD_REQ_TELL_WORKER, GetSequence(), oOutMsgBody);
    return(CMD_STATUS_RUNNING);
}

E_CMD_STATUS StepTellWorker::Callback(
        const tagChannelContext& stCtx,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody,
        void* data)
{
    if (ERR_OK == oInMsgBody.rsp_result().code())
    {
        TargetWorker oInTargetWorker;
        if (oInTargetWorker.ParseFromString(oInMsgBody.data()))
        {
            LOG4CPLUS_DEBUG_FMT(GetLogger(), "AddNodeIdentify(%s, fd %d, seq %llu)!",
                            oInTargetWorker.worker_identify().c_str(), stCtx.iFd, stCtx.ulSeq);
            AddNamedChannel(oInTargetWorker.worker_identify(), stCtx);
            AddNodeIdentify(oInTargetWorker.node_type(), oInTargetWorker.worker_identify());
            SendTo(stCtx);
            return(CMD_STATUS_COMPLETED);
        }
        else
        {
            LOG4_ERROR("error %d: WorkerLoad ParseFromString error!", ERR_PARASE_PROTOBUF);
            return(CMD_STATUS_FAULT);
        }
    }
    else
    {
        LOG4_ERROR("error %d: %s!", oInMsgBody.rsp_result().code(), oInMsgBody.rsp_result().msg().c_str());
        return(CMD_STATUS_FAULT);
    }
}

E_CMD_STATUS StepTellWorker::Timeout()
{
    LOG4_ERROR("timeout!");
    return(CMD_STATUS_FAULT);
}

} /* namespace neb */
