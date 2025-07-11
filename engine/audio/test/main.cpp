#include <print>
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "core/Logger.hpp"
#include "core/SmartReference.hpp"
#include "core/implement/ReferenceCountedDebugger.hpp"
#include "core/AudioEngine.hpp"
#include "win32/base.hpp"

static int test() {
	core::SmartReference<core::IAudioEngine> endpoint;
	if (!core::IAudioEngine::create(endpoint.put())) {
		core::Logger::error("core::IAudioEngine::create");
		return 1;
	}

	if (!endpoint->refreshAudioEndpoint()) {
		core::Logger::error("core::IAudioEngine::refreshAudioEndpoint");
		return 1;
	}

	for (uint32_t i = 0; i < endpoint->getAudioEndpointCount(); i++) {
		auto const name = endpoint->getAudioEndpointName(i);
		core::Logger::info("{}: {}", i, name);
		if (!endpoint->setAudioEndpoint(name)) {
			core::Logger::error("core::IAudioEngine::setAudioEndpoint('{}')", name);
			return 1;
		}
	}

	if (!endpoint->setAudioEndpoint("")) {
		core::Logger::error("core::IAudioEngine::setAudioEndpoint('')");
		return 1;
	}

	core::SmartReference<core::IAudioDecoder> decoder;
	if (!core::IAudioDecoder::create("D:/Project/东方远空界/game/THlib/music/New.ogg", decoder.put())) {
		core::Logger::error("core::IAudioDecoder::create");
		return 1;
	}

	core::SmartReference<core::IAudioPlayer> player;
	if (!endpoint->createStreamAudioPlayer(decoder.get(), core::AudioMixingChannel::music, player.put())) {
		core::Logger::error("core::IAudioEngine::createAudioPlayer");
		return 1;
	}

	player->setLoop(true, 13.015, 141.874 - 13.015);
	player->play(0.0);

	std::string command;
	while (true) {
		std::cin >> command;
		if (command == "exit") {
			break;
		}
		if (command == "play") {
			player->play(0.0);
		}
		if (command == "stop") {
			player->stop();
		}
		if (command == "pause") {
			player->pause();
		}
		if (command == "resume") {
			player->resume();
		}
	}
}

int main() {
	auto const logger = spdlog::stdout_color_mt("core");
	spdlog::set_default_logger(logger);

	win32::set_logger_writer([](std::string_view const message) -> void {
		core::Logger::error(message);
	});

	test();

	core::implement::ReferenceCountedDebugger::reportLeak();
	return 0;
}
