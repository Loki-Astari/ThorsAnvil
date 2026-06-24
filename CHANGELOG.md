# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/), and this project adheres to [Semantic Versioning](https://semver.org/).

## [11.0.0] - 2026-06-24

### Added (ThorsMongo)
- Allow multiple updates in one statement

### Added (NisseBolt)
- Added Shortcut handling

### Added (Mug)
- Added signal handlers for graceful shutdown to improved restart/stop behavior

### Changed (Nisse)
- Ensure URL paths have leading slash (even if not provided by user)
- Improved stream handling
- Fixed `getContentSize()`

### Fixed
- Fixed SSL integration tests to use local certs instead of Let's Encrypt (ThorsSocket)

## [10.2.2] - 2026-05-23

### Improved
- Improved build tools to handle conan build.

## [10.2.1] - 2026-05-22

### Fixed
- Fixed build for Linux.
  - Magic_Enum specified as dependency in Makefile.

## [10.2.0] - 2026-05-21

### Breaking Changes (ThorsSlack)
- Removed `Slack` prefix from header files already in the `Slack` namespace:
  - `SlackClient.h` → `Client.h`
  - `SlackEventHandler.h` → `EventHandler.h`
  - `SlackBlockKit.h` → `BlockKit.h`
- Renamed `EventCallback*.h` headers to shorter, clearer names:
  - `EventCallbackMessage.h` → `EventMessage.h`
  - `EventCallbackAppMentioned.h` → `EventAppMentioned.h`
  - `EventCallbackChannel.h` → `EventsChannel.h`
  - And similarly for all other `EventCallback*` headers
- Removed `APIDialog.h` (superseded by Views API)

### Breaking Changes (NisseBolt)
- Renamed `Handlers.h` → `Runners.h`; handler type aliases renamed accordingly (e.g. `MessageHandler` → `MessageRunner`, `SlashCommandHandler` → `SlashCommandRunner`)

### Improved
- Views can now be constructed with Action Handlers directly
- Moved to value semantics for handler registration
- Simplified event handler addition
- Improved CheckBoxes handling
- Fixed const correctness throughout
- Optimized binary serialization for `std::map`
- Simplified `getString()` from input values

## [10.1.1] - 2026-05-16

### Bug Fix
- Minor bug fixes, needed to support existing apps.

## [10.0.6] - 2026-05-16

### Added
- NisseBolt library: Slack Bolt-style framework for building Slack bots
  - Event handlers for all Slack event types
  - Slash command handling and acknowledgement
  - View support: `viewPush()`, `viewUpdate()`, modal handling
  - User action handlers with value extraction
  - NisseBolt::App framework for bot lifecycle

### Changed
- Removed ThorsIOUtil dependency from ThorsLogging
- Updated build tools and Makefile structure

## [9.1.14] -
- Improve build tools.
- Add install scripts for CMake files.
- Support CMake files for other projects.
- License changed from GPL-3.0 to MIT
- Updated Conan recipe with correct dependencies and package info

## [9.1.7] - 2026-03-22

### Changed
- Removed Slack from Mug (Mug)
- Updated build tools
- Updated logging to be consistent across projects (Nisse)

## [9.1.5] - 2026-03-09

### Fixed
- Applied multiple code fixes

## [9.1.4] - 2026-03-04

### Fixed
- Fixed Windows build

## [9.1.3] - 2026-03-04

### Fixed
- Fixed Homebrew build issue

## [9.1.2] - 2026-03-03

### Fixed
- Various fixes and updates

## [9.0.16] - 2026-01-23

### Changed
- Updated to latest version

## [9.0.11] - 2026-01-18

### Fixed
- Fixed Windows build

## [9.0.08] - 2026-01-17

### Fixed
- Fixed Linux build issues
- Fixed header-only build
- Added ThorsMug and ThorsSlack to header-only variant

## [9.0.01] - 2026-01-15

### Changed
- Major refactor of header files
- Added Nisse (coroutine-based async HTTP server)
- Added NisseHTTP and NisseServer components
- Added ThorsMug (hot-loadable plugin server)
- Added ThorsSlack (Slack REST/webhook client)

## [8.0.01] - 2025-12-15

### Changed
- Modified numeric values of named logging levels
