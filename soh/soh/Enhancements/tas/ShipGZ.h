#pragma once
/*
 * ShipGZ - a GZ-style tool-assisted-speedrun toolkit for Ship of Harkinian.
 *
 * Mirrors the macro / movie engine of gz (github.com/glankk/gz, src/gz/gz.c, gz_movie.c, gz_macro.c):
 *   - pause / frame advance               (gz: frames_queued, state_main_hook)
 *   - input macro recording and playback  (gz: input_hook, movie_input)
 *   - RNG reseed recording / restoring    (gz: srand_hook, movie_seed)
 *   - deterministic audio RNG for macros  (gz: afx_rand_hook)
 *   - savestates that carry the macro frame, rerecord counting, quick record / quick play
 *   - virtual controllers and bindable commands
 *
 * The C entry points below are called from the decomp code (graph.c, padmgr.c, z_play.c,
 * code_800EC960.c) and from the SoH savestate manager.
 */

#include <stdint.h>
#include <libultraship/libultra/controller.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --- main loop (graph.c RunFrame) --- */

/* Called once per host frame before the game would run. Returns non-zero if a game frame should be
 * executed, zero if the game is paused and only the previous frame should be re-presented. */
int32_t ShipGZ_BeginFrame(void);
/* Called after a game frame ran. Returns non-zero if the game should be reset (recorded reset). */
int32_t ShipGZ_EndFrame(void);

/* --- input (padmgr.c PadMgr_ProcessInputs) --- */

/* Called with the freshly polled controller pads, before PadMgr derives press/release edges.
 * Applies command masking, virtual controllers, and macro recording / playback to pads[]. */
void ShipGZ_ProcessPads(OSContPad* pads, int32_t numPads);

/* --- determinism hooks --- */

/* Play_Init reseed. Returns the seed that must be used. */
uint32_t ShipGZ_FilterRandSeed(uint32_t naturalSeed);
/* Gameplay-relevant audio RNG (ocarina memory game). Returns the value that must be used. */
uint32_t ShipGZ_FilterAudioRandom(uint32_t naturalValue);
/* Non-zero while a macro is recording or playing back. */
int32_t ShipGZ_IsMovieActive(void);
/* Non-zero while paused or frame advancing. */
int32_t ShipGZ_IsFrozen(void);
/* Playback speed multiplier for the frame limiter (1.0 = normal). */
float ShipGZ_GetSpeed(void);

/* --- savestate integration (savestates.cpp) --- */
void ShipGZ_OnStateSaved(uint32_t slot);
void ShipGZ_OnStateLoaded(uint32_t slot);

/* --- RNG state accessors (code_800FD970.c) --- */
uint32_t Rand_GetState(void);
void Rand_SetState(uint32_t state);

#ifdef __cplusplus
}
#endif
