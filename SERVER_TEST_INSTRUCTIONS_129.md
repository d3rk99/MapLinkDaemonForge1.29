# Server Test Instructions for Agent 2

## Install

1. Copy the build drop contents from `C:\MapLink129_BuildDrop` to the test server mod directory.
2. Add the `.bikey` from the drop `keys` folder to the server `keys` folder.
3. Launch the server with the packed MapLinkDaemonForge mod set and required dependencies.
4. Let MapLink load its config once, then confirm `ConfigVersion` is `2`.

## Required Toggles

Start with all diagnostics toggles enabled:

- `EnableTransferDebugLogs = true`
- `EnableInventoryRestore = true`
- `EnableCargoRestore = true`
- `EnableHandsRestore = true`
- `EnableContainerRestore = true`
- `EnableAttachmentRestore = true`
- `EnableSafeRestoreMode = true`

## Transfer Test

1. Join the source server with a character carrying:
   - Item in hands.
   - Clothing attachments.
   - Cargo items.
   - A container with nested cargo.
   - A weapon or item with attachments if available.
2. Use a MapLink terminal or configured transfer object.
3. Confirm server RPT contains `[MapLink129] transfer started`.
4. Confirm source RPT logs source server, target server, UID/Steam64, `SavePlayerToU start`, `SavePlayerToU success`, `redirect sent`, and source cleanup.
5. Join the target server after redirect.
6. Confirm target RPT logs `UniversalAPI load start`, `UniversalAPI load success`, `inventory restore start`, item restore/skip lines, and `inventory restore end`.

## Isolation Tests

If the restore fails, repeat with one toggle disabled at a time:

- Disable `EnableHandsRestore` to isolate hands issues.
- Disable `EnableAttachmentRestore` to isolate attachment slot issues.
- Disable `EnableCargoRestore` to isolate cargo placement.
- Disable `EnableContainerRestore` to isolate nested container recursion.
- Keep `EnableSafeRestoreMode = true` while isolating.

## Evidence To Capture

Send Agent 1:

- Source and target RPT snippets containing `[MapLink129]`.
- Player UID/Steam64 used.
- Source server ID and target server ID.
- Inventory items that restored correctly.
- First item skipped or first restore failure reason.
- Whether redirect occurred before or after `SavePlayerToU success`.
