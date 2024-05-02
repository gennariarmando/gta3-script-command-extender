#include "plugin.h"
#include "CRunningScript.h"
#include "CTheScripts.h"
#include "CMessages.h"

class __declspec(dllexport) ScmExtenderIII {
public: 
    static inline std::unordered_map<int32_t, int8_t(*)(int32_t*)> mapOfNewOpcodes = {};

    static void AddOneCommand(int32_t command, int8_t(*func)(int32_t*)) {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

        auto f = mapOfNewOpcodes.find(command);

        if (f == mapOfNewOpcodes.end()) {
            assert("Command already defined");
            std::cout << "Command:" << command << " " << "Already defined" << std::endl;
        }
        else {
            mapOfNewOpcodes[command] = func;
        }
    }

    static int8_t ProcessOneCommand(CRunningScript* script) {
        ++CTheScripts::CommandsExecuted;

        int32_t m_nPrevIp = script->m_nIp;
        int32_t command = CTheScripts::ScriptSpace[script->m_nIp + 1] << 8 | CTheScripts::ScriptSpace[script->m_nIp];
        script->m_nIp += sizeof(uint16_t);
        script->m_bNotFlag = (command & 0x8000);
        command &= 0x7FFF;

        tScriptParam* params = CTheScripts::ScriptParams;
        auto f = mapOfNewOpcodes.find(command);
        if (f != mapOfNewOpcodes.end()) {
            script->CollectParameters((int32_t*)&script->m_nIp, 32);
            auto ret = f->second((int32_t*)params);
            script->StoreParameters((int32_t*)&script->m_nIp, 32);
            return ret;
        }

        script->m_nIp = m_nPrevIp;
        --CTheScripts::CommandsExecuted;

        return script->ProcessOneCommand();
    }

    ScmExtenderIII() {
        plugin::Events::initRwEvent += []() {
            CTheScripts::ScriptSpace = plugin::patch::Get<uint8_t*>(0x4387A0 + 3);
        };

        auto processScriptHook = [](CRunningScript* script, void*) {
            if (script->m_bUseMissionCleanup)
                script->DoDeathArrestCheck();

            if (script->m_bIsMission && CTheScripts::FailCurrentMission == 1 && script->m_nSP == 1)
                script->m_nIp = script->m_anStack[--script->m_nSP];

            if (CTimer::m_snTimeInMilliseconds >= script->m_nWakeTime) {
                while (!ProcessOneCommand(script)) {
                }
            }

            if (!script->m_bAwake)
                return;

            if (!(CPad::GetPad(0)->NewState.ButtonCross && !CPad::GetPad(0)->OldState.ButtonCross))
                return;

            script->m_nWakeTime = 0;

            for (int i = 0; i < 6; i++) {
                if (CMessages::BIGMessages[i].m_Stack[0].m_pText != nullptr)
                    CMessages::BIGMessages[i].m_Stack[0].m_nStartTime = 0;
            }

            if (CMessages::BriefMessages[0].m_pText != nullptr)
                CMessages::BriefMessages[0].m_nStartTime = 0;
        };
        plugin::patch::RedirectCall(0x4393DF, LAMBDA(void, __fastcall, processScriptHook, CRunningScript*, void*));
    }
} scmExtenderIII;
