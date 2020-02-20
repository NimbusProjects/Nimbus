#include "hvhtab.h"

#include "../../Hacks/antiaim.h"
#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void HvH::RenderTab()
{
    const char* aTypes[] = {
            "Rage", "Legit", "Advanced", "Custom"
    };



  	const char* resType[] = {
  		"LBY", "LBY flip", "pResolver", "Test"
  	};


    ImGui::Columns(2, nullptr, true);
    {
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), true);
        {
            ImGui::Checkbox(XORSTR("Anti-Aim"), &Settings::AntiAim::enabled);
            ImGui::Separator();
            //ImGui::Combo(XORSTR("Mode"), (int*)& Settings::AntiAim::type, aTypes, IM_ARRAYSIZE(aTypes));
            ImGui::Checkbox(XORSTR("State Based Anti-Aim"), &Settings::AntiAim::States::enabled);
            ImGui::Separator();
            if (!Settings::AntiAim::States::enabled) {
                ImGui::Combo(XORSTR("Mode"), (int*)& Settings::AntiAim::type, aTypes, IM_ARRAYSIZE(aTypes));
                ImGui::Separator();
            }
            else
            {
                ImGui::Combo(XORSTR("Stand-type"), (int*)& Settings::AntiAim::States::Stand::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Stand::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##STANDANGLE"), &Settings::AntiAim::States::Stand::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
/*
                ImGui::Combo(XORSTR("Walk-type (IN_WALK flag broken)"), (int*)& Settings::AntiAim::States::Walk::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Walk::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##WALKANGLE"), &Settings::AntiAim::States::Walk::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
*/
                ImGui::Combo(XORSTR("Run-type"), (int*)& Settings::AntiAim::States::Run::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Run::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##RUNANGLE"), &Settings::AntiAim::States::Run::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                ImGui::Combo(XORSTR("Air-type"), (int*)& Settings::AntiAim::States::Air::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Air::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##AIRANGLE"), &Settings::AntiAim::States::Air::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
            }
            ImGui::Text(XORSTR("Anti-Aim Keys"));
            ImGui::Separator();

            ImGui::Columns(2, nullptr, true);
            {
                ImGui::Text(XORSTR("Left-key"));
                ImGui::Text(XORSTR("Right-key"));
            }
            ImGui::NextColumn();
            {
                UI::KeyBindButton(&Settings::AntiAim::left);
                UI::KeyBindButton(&Settings::AntiAim::right);
            }
            ImGui::Columns(1);
            ImGui::Separator();

            ImGui::Text(XORSTR("Disable"));
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Knife"), &Settings::AntiAim::AutoDisable::knifeHeld);
            ImGui::Separator();

            ImGui::Columns(2, nullptr, true);
            {
                ImGui::Separator();
                ImGui::Checkbox(XORSTR("Freestand"), &Settings::AntiAim::Freestanding::enabled);
                ImGui::Separator();
				        //ImGui::SliderFloat(XORSTR("##FAKELATENCYAMOUNT"), &Settings::FakeLatency::value, 1, 999, XORSTR("Fakelatency Amount: %0.f"));
                ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
                ImGui::Checkbox(XORSTR("Fake Lag on b$end"), &Settings::FakeLag::bSend);
                if (Settings::FakeLag::enabled)
                {
                    ImGui::Checkbox(XORSTR("Fake Lag States"), &Settings::FakeLag::States::enabled);
                    ImGui::Checkbox(XORSTR("Fake Lag On Peek"), &Settings::FakeLag::lagSpike);
                }
            }
            ImGui::NextColumn();
            {
                if (Settings::FakeLag::enabled)
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 16, XORSTR("FakeLag Amount: %0.f"));
				//ImGui::SliderInt(XORSTR("Min FakeLag Ticks"), &Settings::FakeLag::minTicks, 0, 128, XORSTR("Delta Min FakeLag %0.f"));
				//ImGui::SliderInt(XORSTR("Max FakeLag Ticks"), &Settings::FakeLag::maxTicks, 0, 2048, XORSTR("Delta Max FakeLag: %0.f"));
                    ImGui::PopItemWidth();
                }
            }
            ImGui::Separator();

            if (Settings::FakeLag::States::enabled) {
                ImGui::Columns(2, nullptr, true);
                {
                    ImGui::Checkbox(XORSTR("Stand Fake Lag"), &Settings::FakeLag::States::Standing::enabled);
                    ImGui::Checkbox(XORSTR("Move Fake Lag"), &Settings::FakeLag::States::Moving::enabled);
                    ImGui::Checkbox(XORSTR("In-air Fake Lag"), &Settings::FakeLag::States::Air::enabled);
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##STANDFAKELAGAMOUNT"), &Settings::FakeLag::States::Standing::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##MOVEFAKELAGAMOUNT"), &Settings::FakeLag::States::Moving::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##INAIRFAKELAGAMOUNT"), &Settings::FakeLag::States::Air::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                }
            }
               if (Settings::AntiAim::type == AntiAimType::CUSTOM || !Settings::AntiAim::States::enabled) {
                ImGui::Separator();

            ImGui::Checkbox(XORSTR("Desync Swap Status"), &Settings::AntiAim::swap);
            ImGui::EndChild();
        }
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), true);
        {

            ImGui::Text(XORSTR("Resolver"));
            ImGui::Checkbox(XORSTR("Resolve All"), &Settings::Resolver::resolveAll);
            if(Settings::Resolver::resolveAll)
            ImGui::SliderFloat(XORSTR("##DELT"), &Settings::Resolver::resolveDelt, 0, 1, "Resolver Delta %.2f");
            ImGui::Separator();

//	          if(Settings::Resolver::resolveAll == true)
//			        ImGui::Combo("Resolver Type",(int*)&Settings::Resolver::type, resType, IM_ARRAYSIZE(resType));

            ImGui::Separator();
            ImGui::Text(XORSTR("Movement"));
            ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Aimbot::AutoCrouch::enabled);
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Angle Indicator"), &Settings::AngleIndicator::enabled);
            ImGui::Checkbox(XORSTR("LBY Breaker"), &Settings::AntiAim::LBYBreaker::enabled);
           // if( Settings::AntiAim::LBYBreaker::enabled ){
             //   ImGui::SliderFloat(XORSTR("##LBYOFFSET"), &Settings::AntiAim::LBYBreaker::offset, 0, 360, "LBY Offset(from fake): %0.f");
            //}
            if (Settings::AntiAim::LBYBreaker::enabled) {
                ImGui::Checkbox(XORSTR("Custom Breaker Angle"), &Settings::AntiAim::LBYBreaker::custom);
            }
            if (Settings::AntiAim::LBYBreaker::enabled && Settings::AntiAim::LBYBreaker::custom) {
                ImGui::SliderFloat(XORSTR("##LBYOFFSET"), &Settings::AntiAim::LBYBreaker::offset, 0, 360, "LBY Offset(from fake): %1.0f");
            }
                ImGui::SliderFloat(XORSTR("##DESYNC"), &Settings::AntiAim::Desync::amount, 0, 360, "Desync Offset %0.f");
                ImGui::Separator();
                ImGui::Text("!CAREFUL WITH THIS! \nUsable with the Advanced/Rage AntiAim type. \nGL HF gamers :))");
                //ImGui::SliderFloat(XORSTR("##DTIME"), &Settings::AntiAim::Desync::time, 0.25, 4.2, "Desync Time %.3f");
                //ImGui::SliderFloat(XORSTR("##DINTERVAL"), &Settings::AntiAim::Desync::interval, 0.010, 5.5, "Desync Interval %.3f");
                //ImGui::SliderFloat(XORSTR("##DOFF1"), &Settings::AntiAim::off1, 0.1, 360, "Desync Random Offset 1 %.3f");
                ImGui::SliderFloat(XORSTR("##DOFF2"), &Settings::AntiAim::off2, 0.1, 56, "Desync Random Offset %.3f");
                //ImGui::SliderFloat(XORSTR("##ROFF1"), &Settings::AntiAim::roff1, 0.1, 360, "Real Random Offset 1 %.3f");
                ImGui::SliderFloat(XORSTR("##ROFF2"), &Settings::AntiAim::roff2, 0.1, 56, "Real  Random Offset %.3f");

                ImGui::Separator();
                ImGui::Text("Not implemented yet...");
                ImGui::Text("!CHANGES LEGIT AA ANGLES CAN SEE ON OVERWATCH!");
                ImGui::SliderInt(XORSTR("##FakeL"), &Settings::AntiAim::fakeL, 0, -360, "Left Fake %0.f");
                ImGui::SliderInt(XORSTR("##FakeR"), &Settings::AntiAim::fakeR, 0, 360, "Right Fake %0.f");

            ImGui::Separator();

				    ImGui::Checkbox(XORSTR("BackTrack"), &Settings::BackTrack::enabled);
            if(Settings::BackTrack::enabled)
            {
        ImGui::Checkbox(XORSTR("BackTrack Chams Enabled"), &Settings::BackTrack::Chams::enabled);
				ImGui::Checkbox(XORSTR("BackTrack Draw Last Only"), &Settings::BackTrack::Chams::drawlastonly);
				ImGui::SliderInt(XORSTR("##LAGCOMP"), &Settings::BackTrack::time, 2.f, 24.f, XORSTR("LagComp: %0.2f TICKS"));
				ImGui::SliderFloat(XORSTR("##LAGCOMPFOV"), &Settings::BackTrack::fov, 1.f, 180.f, XORSTR("LagComp FOV: %0.2f"));

            }
        }
            ImGui::EndChild();
    }
}
}
