#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <random>
#include <cmath>

using namespace geode::prelude;

class $modify(DecoEditorUI, EditorUI) {
	bool init(LevelEditorLayer* lel) {
		if (!EditorUI::init(lel)) return false;

		// "undo-menu" ID comes from the geode.node-ids dependency
		if (auto menu = this->getChildByID("undo-menu")) {
			auto spr = ButtonSprite::create("Deco", 40, true, "bigFont.fnt", "GJ_button_04.png", 30.f, .6f);
			auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(DecoEditorUI::onDeco));
			btn->setID("deco-button"_spr);
			menu->addChild(btn);
			menu->updateLayout();
		}
		return true;
	}

	void onDeco(CCObject*) {
		auto sel = this->getSelectedObjects();
		if (!sel || sel->count() == 0) {
			FLAlertLayer::create("Quick Decorator", "Select some objects first, then press <cy>Deco</c>.", "OK")->show();
			return;
		}

		auto mod = Mod::get();
		int count = static_cast<int>(mod->getSettingValue<int64_t>("count"));
		float radius = static_cast<float>(mod->getSettingValue<double>("radius"));
		float sMin = static_cast<float>(mod->getSettingValue<double>("min-scale"));
		float sMax = static_cast<float>(mod->getSettingValue<double>("max-scale"));
		bool rot = mod->getSettingValue<bool>("random-rotation");
		if (sMax < sMin) std::swap(sMin, sMax);

		static std::mt19937 rng{std::random_device{}()};
		std::uniform_real_distribution<float> angle(0.f, 6.2831853f), dist(0.f, 1.f), scale(sMin, sMax), deg(0.f, 360.f);

		// copy the list first so we don't iterate while adding objects
		std::vector<GameObject*> targets;
		for (auto obj : CCArrayExt<GameObject*>(sel)) targets.push_back(obj);

		int made = 0;
		for (auto obj : targets) {
			auto base = obj->getPosition();
			for (int i = 0; i < count; i++) {
				float a = angle(rng);
				float r = std::sqrt(dist(rng)) * radius; // even spread over the disc
				CCPoint pos = base + CCPoint{std::cos(a) * r, std::sin(a) * r};
				auto piece = m_editorLayer->createObject(obj->m_objectID, pos, false);
				if (!piece) continue;
				piece->setScale(scale(rng));
				if (rot) piece->setRotation(deg(rng));
				made++;
			}
		}

		Notification::create(fmt::format("Added {} decoration pieces", made), NotificationIcon::Success)->show();
	}
};
