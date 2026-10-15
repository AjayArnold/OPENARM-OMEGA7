The easiest way is to use your fork for the OpenArm side, since it already has the Servo changes built in.

**1. Make a workspace and clone both repos into `src/`:**
```bash
mkdir -p ~/omega7_ws/src && cd ~/omega7_ws/src
git clone -b servo-teleop https://github.com/AjayArnold/OPENARM-OMEGA7.git
git clone https://github.com/rrndomingo2003/openarm_ros2.git
```
`colcon` will find `omega7_ros2` inside the first repo on its own. The `moveit_config_changes/` folder has no `package.xml`, so it gets ignored, which is what we want. Your fork's version of those files is the one that gets used.

**2. Install dependencies and build:**
```bash
cd ~/omega7_ws
rosdep install --from-paths src --ignore-src -y
colcon build
source install/setup.bash
```

**3. Run it** with the commands in the "How to run (fake hardware)" section of STATUS.md: the launch file, the move-off-singularity command, then `omega7_node` and `omega7_teleop`.

Things he'll need to check:

- **Fork access:** if your fork is private, add him as a collaborator there too. If he'd rather use upstream `openarm_ros2`, he has to copy the three files from `moveit_config_changes/` into `openarm_bimanual_moveit_config/launch/` and `config/` himself. `joint_limits.yaml` isn't in the commit yet, so he'd also need that file from your fork.
- **Omega.7 SDK:** `omega7_node` talks to the hardware through the Force Dimension SDK, which rosdep won't install. He'll need it set up wherever your `CMakeLists.txt` expects it. If he's only testing teleop logic, he still needs the node running to publish Omega messages.
- **MoveIt Servo:** it has to be installed for his ROS 2 distro. rosdep should handle this if the fork's `package.xml` declares it.
- **The `install/` gotcha:** point him to that section of STATUS.md. After editing a config file, he should check that `install/` actually updated.

Adding STATUS.md to the repo would help him a lot, since it covers all the running and tuning details.
