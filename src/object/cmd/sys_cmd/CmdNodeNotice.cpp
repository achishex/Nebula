/*******************************************************************************
 * Project:  Nebula
 * @file     CmdNodeNotice.cpp
 * @brief 
 * @author   Bwar
 * @date:    2016年8月16日
 * @note
 * Modify history:
 ******************************************************************************/

#include "util/json/CJsonObject.hpp"
#include "CmdNodeNotice.hpp"

namespace neb
{

CmdNodeNotice::CmdNodeNotice()
    : pStepNodeNotice(NULL)
{
}

CmdNodeNotice::~CmdNodeNotice()
{
}

bool CmdNodeNotice::AnyMessage(
                const tagChannelContext& stCtx,
                const MsgHead& oInMsgHead,
                const MsgBody& oInMsgBody)
{
    CBuffer oBuff;
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;

    CJsonObject oJson;
    if (oJson.Parse(oInMsgBody.data()))
    {
        LOG4_DEBUG("CmdNodeNotice seq[%llu] jsonbuf[%s] Parse is ok",
            oInMsgHead.seq(),oInMsgBody.data().c_str());

        Step* pStep = new StepNodeNotice(oInMsgBody);
        if (pStep == NULL)
        {
            LOG4_ERROR("error %d: new StepNodeNotice() error!", ERR_NEW);
            return(CMD_STATUS_FAULT);
        }

        if (Register(pStep))
        {
            if (CMD_STATUS_RUNNING != pStep->Emit(ERR_OK))
            {
                Remove(pStep);
            }
            return(true);
        }
        else
        {
            delete pStep;
        }
    }
    else
    {
        LOG4_ERROR("failed to parse %s", oInMsgBody.data().c_str());
    }

    return(false);
}

} /* namespace neb */
