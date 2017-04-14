#include <StdAfx.h>
#include <FlowSystem\Nodes\FlowBaseNode.h>

class CFlowNode_Test_Flow : public CFlowBaseNode<eNCT_Singleton>
{
public:
	CFlowNode_Test_Flow(SActivationInfo* pActInfo)
	{
	};

	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig<int>("in", _HELP("your_help_text")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig<int>("out", _HELP("your_help_text")),
			{ 0 }
		};
		config.sDescription = _HELP("Test Flownode Description (So Helpful~~~)");
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo)
	{
		switch (event)
		{
		case eFE_Activate:
			int in = GetPortInt(pActInfo, 0);
			ActivateOutput(pActInfo, 0, in);
			break;
		};
	}
};

REGISTER_FLOW_NODE("OpenDiva:Test_Flow", CFlowNode_Test_Flow);

/*
TODO:
Add opendiva flownodes for the following:
-Lyrics (Lyrics, Romaji, Translated) - Text (eFE_Update)
-Song Start/End - Boolean - reactinary? (eFE_Activate) or every tick?  (eFE_Update)
-HP Text/Value - Text/Integer  (eFE_Update)
-Score Text/Value - Text/Integer  (eFE_Update)
-Completion Value (current num notes, total notes, percentage) - Integer, Integer, Float  (eFE_Update)
-ChanceTime Enter/Exit - Boolean - reactionary? (eFE_Activate) or every tick?  (eFE_Update)
-Technical Zone Enter/Exit - Boolean - reactionary? (eFE_Activate) or every tick?  (eFE_Update)
-Technical Zone (Notes Left, Active) - Integer, Boolean  (eFE_Update)
-Get Canvas ID - Integer (eFE_Activate)
*/