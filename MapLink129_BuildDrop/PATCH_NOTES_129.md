# MapLinkDaemonForge DayZ 1.29 Diagnostics Patch

Branch: `maplink-1.29-diagnostics`

## Scope

This is a diagnostics/safety patch for DayZ 1.29 testing. It does not remove MapLink features and keeps default behavior enabled through config toggles.

## Added Config Toggles

The MapLink config version is bumped to `2` and these defaults are added:

- `EnableTransferDebugLogs = true`
- `EnableInventoryRestore = true`
- `EnableCargoRestore = true`
- `EnableHandsRestore = true`
- `EnableContainerRestore = true`
- `EnableAttachmentRestore = true`
- `EnableSafeRestoreMode = true`

## Diagnostics Logs

All new diagnostics use the `[MapLink129]` prefix.

Coverage added for:

- Transfer start, source server, target server, player UID/Steam64.
- `SavePlayerToU` start, success, and failure paths.
- Redirect RPC sent/failure paths.
- Source player cleanup and body removal.
- UniversalAPI player load start, success, empty, parse failure, invalid data, and API failure.
- Inventory restore start/end.
- Item restored, item skipped, and restore failure reason.

## Safety Behavior

The MapLink-side `UApiEntityStore` restore override now:

- Checks class names before spawn attempts.
- Checks parent, inventory, player, and human inventory references before placement.
- Respects restore toggles for inventory, cargo, hands, containers, and attachments.
- Logs and continues item-by-item when `EnableSafeRestoreMode` is true.
- Avoids aborting the full restore when one stored item cannot be recreated.

## Code Areas Identified

- Transfer terminal use: `ActionOpenMapLinkMenu.c`, `MapLinkOpenOnAny.c`, `Hive_Terminal.c`, `DepaturePointMenu.c`, `DeparturePointWidget.c`.
- Player save to UniversalAPI: `SourceCode/MapLink/scripts/4_World/entities/PlayerBase.c`.
- Player redirect/connect: `SourceCode/MapLink/scripts/4_World/entities/PlayerBase.c`, `SourceCode/MapLink/scripts/5_Mission/mission/ModdedMissionGameplay.c`, `SourceCode/_MapLinkBase/scripts/3_Game/DayZGame.c`.
- Source player cleanup: `SourceCode/MapLink/scripts/4_World/entities/PlayerBase.c`, `SourceCode/MapLink/scripts/5_Mission/mission/MissionServer.c`.
- Player load from UniversalAPI: `SourceCode/MapLink/scripts/5_Mission/mission/MissionServer.c`.
- Inventory/hands/cargo/attachments/nested containers: `SourceCode/MapLink/scripts/4_World/entities/PlayerDataStore.c` and `SourceCode/MapLink/scripts/4_World/entities/UApiEntityStore_MapLink129.c`.
