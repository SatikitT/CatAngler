#pragma once

#include "Fish.h"
#include "Vector2D.h"
#include "FishingRod.h"
#include "Collider.h"
#include <vector>
#include "SDL.h"



class FishingManager {
    private:
        FishingRod* rod = new FishingRod(1,"rod","Equipment", "");
        std::vector<Fish*> fishInArea;

        Collider* m_Collider;
        static FishingManager* s_Instance;

    public:
        FishingManager(std::vector<Fish*> fishInArea = {});

        inline static FishingManager* GetInstance() { return s_Instance = (s_Instance != nullptr) ? s_Instance : new FishingManager(); }

        void checkFishingPosition(int  x,int y, std::string map);

        void update(bool input, int x, int y);
};