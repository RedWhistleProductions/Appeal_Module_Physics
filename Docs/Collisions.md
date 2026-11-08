# 2D and 3D collisions

Physics now uses Box2D 3.2.0 for 2D and Jolt 5.6.0 for 3D. Both use the same body, stepping, contact, event, filter, and trigger interfaces. A body belongs to one plugin world; different plugin worlds do not collide with each other.

## Basic

```
Add_Dynamic_Module Physics Appeal_Box2D_Physics
Physics Set_Units_Per_Meter 64
Physics Set_Gravity 0 -1800 0
Physics Use Floor Default
Physics Set_Body_Type Floor Static
Physics Set_Box Floor 1024 64
Physics Set_Position Floor 512 -32 0
Physics Use Player Default
Physics Set_Capsule Player 24 150
Physics Set_Position Player 200 200 0
Physics Step 0.016666667
Print (Physics Get Player Grounded)
```

For 3D, load `Appeal_Jolt_Physics` and use `Set_Box_3D Entity Width Height Depth` or `Set_Sphere Entity Radius`. Capsules are upright along Y in both plugins. `Set_Box` takes full dimensions; capsule height includes both rounded ends. Positions are collider centers, not sprite corners. Coordinates use +Y up by default. Box2D ignores the Z arguments and reports zero for Z queries.

`Set_Units_Per_Meter Number` must precede attaching bodies, and applies to loaded plugins. Default is 1 (meters). Positions, dimensions, velocity, gravity, forces, impulses, and event speeds are expressed using that scale; force is mass times coordinate acceleration. Mass is kilograms, time seconds. Default gravity is 9.8 meters per second squared down Y. Call `Set_Gravity` after selecting units to specify another acceleration. No automatic conversion of screen coordinates or World prototypes occurs.

Create an independent body for each tile *placement*, even when multiple cells share an immutable entity prototype. Graphics can still share textures. Decorative scenery need not have a collider.

## Walking and Mechanics

`Mechanics Set Entity External_Physics 1` disables the default Mechanics plugin's position/gravity/floor integration. It works in all three default orientations and retains selected-entity digital and analog actions. Input still supplies mapped -100..100 values and owns dead zones.

For a flat top-down game use zero gravity and keep its planar movement grounded; 2D Box2D does not simulate the top-down Mechanics plugin's separate Z jump height.

Each frame: apply input/AI actions, copy desired Mechanics velocity into Physics (preserve Physics vertical velocity except when jumping), step Physics once, then copy position/velocity/support back into Mechanics. Subtract the collider's half-height when converting its center to feet position. Set `Grounded` after copying the position. `Mechanics Get Entity Field` returns a value directly, alongside the existing published globals. When external physics is enabled, grounding does not snap the entity to `Ground_Height`.

`Examples/World/Sprite_Garden` uses real tile colliders, a crate platform, upright character capsules, and door sensors for the three looping screens. It preserves the existing graphics and controller/focus behavior. Character capsule height remains 150 pixels while ducking; automatic collider resizing/ceiling clearance is not implemented. Use a separate shorter collider only after checking that the intended shape fits.

`Examples/Physics/Collision_3D` is a finite example: a character walks along Z, lands on a solid floor, reports an impact, and activates a World door destination.

## Triggers, doors, and hits

Set `Sensor` **before** creating a Box2D shape:

```
Physics Use Door Default
Physics Set_Body_Type Door Static
Physics Set Door Sensor 1
Physics Set Door Tag Door
Physics Set_Box Door 32 200
Physics Set_Position Door 500 100 0
```

A sensor reports overlaps without blocking movement. `Physics Touching Player Door` returns 1 while the latest simulation snapshot overlaps. `Get Entity Contact_Count`, `Contact Entity Index Field`, `Get Entity Event_Count`, and `Event Entity Index Field` use zero-based indices. Out-of-range indices return an empty string.

Contact fields: `Other`, `Tag` (the other body's tag), `Sensor`, `Normal_X`, `Normal_Y`, and `Normal_Z` in Jolt. Solid normals point from the other body toward this body. Sensor normals are zero. Events additionally supply `Type` (`Begin`, `End`, or `Hit`) and `Speed` (closing speed). Hit events are solid impacts faster than 1 meter/second; a sensor weapon/hazard can instead apply damage once on `Begin`. Persistent contact is not repeated damage unless the game deliberately implements that policy. Damage, teams, health, door locks, and animation selection remain game logic.

Read snapshots after stepping and before the next step. Events last one step. Names and tags are owned strings and remain safe when another body is removed; removing a touching body queues an End event for the surviving body. Contact snapshots persist for resting bodies. Teleporting or changing shapes/filters takes effect in contact queries after another step. Sensors use discrete overlap detection, so a very fast object can cross a thin sensor between steps; use thicker trigger volumes or smaller steps. `Bullet 1` enables continuous *solid* collision detection; it does not guarantee swept sensor events.

`Examples/Physics/Update_Entity_Mode` defines `Collision_Update_Mode`. Set `Collision_Entity` to a World/Mechanics/Physics entity and call it after stepping. Sensor tags Water, Ladder, Climbable, and Vehicle select Swimming, Ladder, Climbing, and Vehicle modes with explicit precedence. Leaving all such volumes restores Ground. The same code handles a player or NPC. It updates World properties and `Mechanics.<entity>.Mode` for animation scripts; the game chooses the swimming/climbing/vehicle mechanic set and animations.

For a door, react to Begin or Touching, resolve its World destination with `World Get_Door_Destination`, load/use that section, rebuild its colliders, and place the entrant at the configured entry point. Spawn clear of the destination door to avoid immediate re-entry. Sprite Garden demonstrates this with its page-specific spawn positions.

## Advanced

- `Step Seconds`: advances each loaded collision world exactly once. Accepts finite values in (0, 0.1]. Use a fixed timestep/accumulator and smaller steps for fast motion.
- `Update All`: steps each real world once at 1/60 second, regardless of entity count. Legacy plugins retain per-entity Update behavior. `Update Entity` advances the entity's entire plugin world at 1/60; prefer Step or Update All.
- `Set_Body_Type Entity Static|Dynamic|Kinematic`: fixed geometry, simulated objects, or velocity-driven objects. Static/static pairs do not generate contacts. Character bodies lock rotation; this first API exposes translational physics.
- `Set Entity Tag Text`: application metadata. `Sensor 0|1`: overlap-only shape. Box2D requires setting Sensor before creating/replacing a shape.
- `Set Entity Category Integer` / `Mask Integer`: unsigned 64-bit collision filtering (decimal or 0x hexadecimal). Both category/mask tests must pass. Default category 1, mask all bits. Sprite Garden uses character category 2/mask 1 so Adam and Eve do not block one another.
- `Set Entity Friction Number`, `Restitution 0..1`, `Gravity_Scale Number`, `Bullet 0|1`, `Mass PositiveNumber`. Set mass after the shape; replacing a shape can recalculate mass. Box2D also supports `Density PositiveNumber`.
- `Get Entity Position_X|Position_Y|Position_Z|Velocity_X|Velocity_Y|Velocity_Z|Mass|Tag|Grounded|Contact_Count|Event_Count`. Grounded requires a non-sensor support normal opposing gravity (up to 60 degrees from up); zero gravity reports false. It is a support query, not a complete stair/ledge character controller.
- Existing Set_Position, Set_Velocity, Apply_Force, Apply_Impulse, Release, load-plan and multi-plugin commands remain available. Release removes the actual physics body. Shape setters replace the existing single shape. Unknown shapes/queries on an incompatible backend fail explicitly.

No script callbacks execute during simulation. Jolt uses a single-thread job system for deterministic adapter ownership; callback data is copied before scripts read it. Backend capacity is 16384 Jolt bodies. Capacity and invalid numeric/shape errors are reported rather than ignored.

## Globals

Physics query results are returned as expressions; this module creates no new globals. Mechanics continues publishing per-entity state, including Mode and the External_Physics integration flag.

## Build and validation

Run the appropriate `Packaging/Install_Box2D.py` or `Packaging/Install_Jolt.py` inside each plugin with Python 3, GCC/MinGW (C++17), and CMake for Jolt. Downloads are pinned by commit and SHA-256 and install only under `.deps`. Build with `make -f Makefile` in the plugin and Physics module directories. Third-party MIT notices are included beside each plugin. Existing default/test projects remain separate repositories.

`python Tests/Physics_Tests.py` builds native backend tests and, on Windows, checks the installed DLLs through Appeal scripts, including player/NPC mode priorities and Sprite Garden mechanics synchronization. Existing World and Mechanics tests remain applicable. Windows is runtime-tested; this change does not claim macOS runtime validation.
