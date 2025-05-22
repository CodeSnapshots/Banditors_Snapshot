// Copyright (C) 2025 by Haguk Kim

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"


static TAutoConsoleVariable<bool> CVarShowDebugShapes(
    TEXT("TR.ShowDebugShapes"),
    false,
    TEXT("Show debug shapes in editor. 0 = Off, 1 = On"),
    ECVF_Default
);

static TAutoConsoleVariable<bool> CVarShowScreenDebugMsgs(
    TEXT("TR.ShowScreenDebugMsgs"),
    true,
    TEXT("Show debug messages in editor. 0 = Off, 1 = On"),
    ECVF_Default
);

static TAutoConsoleVariable<bool> CVarEnableDebugFeatures(
    TEXT("TR.EnableDebugFeatures"),
    true,
    TEXT("Enable debug features(like instant respawn) in editor. 0 = Off, 1 = On"),
    ECVF_Default
);