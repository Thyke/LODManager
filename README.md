# LOD Manager for Unreal Engine

LOD Manager is a performance optimization plugin for Unreal Engine that provides dynamic Level of Detail (LOD) management for character meshes. It automatically adjusts mesh details, visibility, and physics simulations based on camera distance, enhancing performance while maintaining visual quality.

## Features

- **Dynamic LOD Management**: Automatically adjusts LOD levels based on distance from camera.
- **Visibility Optimization**: Toggles mesh visibility for distant objects.
- **Physics Optimization**: Manages physics simulation based on distance and movement.
- **Cloth Simulation Control**: Enables/disables cloth simulation for distant meshes.
- **Shadow Casting Management**: Controls shadow casting based on distance.
- **Configurable Settings**: Utilizes data-driven design with ULODConfig for easy tweaking.
- **Multi-Mesh Support**: Manages multiple skeletal meshes per character.

## Installation

1. Copy the `LODManager` folder into your Unreal Engine project's `Plugins` directory.
2. Rebuild your project.
3. Enable the LOD Manager plugin in your project settings.

## Usage

### Setting Up LOD Manager

1. Add `UCharacterLODManagerComponent` to your character blueprint or C++ class:

   ```cpp
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
   UCharacterLODManagerComponent* LODManagerComponent;
   ```

2. Initialize the component in your character's constructor:

   ```cpp
   LODManagerComponent = CreateDefaultSubobject<UCharacterLODManagerComponent>(TEXT("LODManagerComponent"));
   ```

### Configuring LOD Settings

1. Create a new LOD Config asset in the Content Browser: Right-click > Miscellaneous > Data Asset > LODConfig
2. Configure the LOD settings in the newly created asset:
   - Set LOD levels and their distance thresholds
   - Adjust update interval, distant limb thresholds, and sleep threshold
   - Enable/disable visibility and physics optimizations

3. Assign the LOD Config asset to your LOD Manager Component in the blueprint or C++:

   ```cpp
   LODManagerComponent->LODConfig = YourLODConfigAsset;
   ```

### Registering Skeletal Meshes

For automatic registration of all skeletal meshes on the character:

- The component will automatically register all skeletal mesh components found on the owner actor in BeginPlay.

For manual registration of specific meshes:

```cpp
LODManagerComponent->RegisterSkeletalMeshComponent(YourSkeletalMeshComponent);
```

## LOD Config Properties

- `LODUpdateInterval`: How often the LOD checks are performed (in seconds)
- `DistantLimbThreshold`: Multiplier for the maximum LOD distance to determine "distant" threshold
- `VeryDistantLimbThreshold`: Multiplier for the maximum LOD distance to determine "very distant" threshold
- `SleepThreshold`: Velocity threshold for putting rigid bodies to sleep
- `LODLevels`: Array of LOD levels with their distance thresholds
- `bEnableVisibilityOptimization`: Toggle for visibility optimization
- `bEnablePhysicsOptimization`: Toggle for physics optimization

## Best Practices

- Fine-tune the LOD Config settings for your specific game requirements.
- Use the LOD Manager on characters or objects that have multiple detail levels or many meshes.
- Monitor performance gains and adjust settings as needed.

## Performance Considerations

- The LOD Manager performs distance checks at regular intervals. Adjust the `LODUpdateInterval` to balance between performance and responsiveness.
- Utilize the physics and visibility optimizations to reduce rendering and simulation load for distant objects.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
