#include "debug.h"
#include <imgui.h>
#include <format>
#include "engine/engine.h"
#include "util.h"

bool ramOpen = true;  // This is bad, but the class is static anyway so idgaf

void Debug::show(const Engine& engine) {
	showProgram(engine);
	showRegs(engine);
	showRam(engine);
}

void Debug::showRam(const Engine& engine) {
	ImGui::Begin("Ram");

	if (ImGui::BeginTable("Ram Table", 17)) {
		ImGui::TableNextColumn();
		ImGui::Text("000");

		for (int i = 0; i < 4096; i++) {
			ImGui::TableNextColumn();

			ImVec4 c = (engine.pc == i || engine.pc + 1 == i)
						   ? ImVec4(0.05, 0.05, 0.7, 1.0)
						   : ImVec4(0, 0, 0, 1);

			ImGui::TextColored(c, util::byteToHex(engine.ram[i]).c_str());

			if ((i + 1) % 16 == 0) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGui::Text(util::addrToHex(i + 1).c_str());
			}
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void Debug::showRegs(const Engine& engine) {
	ImGui::Begin("Registers");

	for (int i = 0; i < 16; i++) {
		ImGui::LabelText(std::format("v{}", util::nibbleToHex(i)).c_str(),
						 util::byteToHex(engine.vreg[i]).c_str());
	}

	ImGui::End();
}

void Debug::showProgram(const Engine& engine) {
	ImGui::Begin("Program");

	ImGui::LabelText("PC", util::addrToHex(engine.pc).c_str());
	ImGui::LabelText("I", util::addrToHex(engine.ireg).c_str());

	ImGui::LabelText("Delay Timer", util::byteToHex(engine.timer).c_str());
	ImGui::LabelText("Sound Timer", util::byteToHex(engine.sound).c_str());

	ImGui::LabelText("Last executed",
					 util::instructionToHex((engine.ram[engine.pc - 2] << 8) +
											engine.ram[engine.pc - 1])
						 .c_str());

	ImGui::End();
}
