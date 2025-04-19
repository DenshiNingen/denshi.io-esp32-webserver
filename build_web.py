import os
import sys
Import("env")

def before_build_spiffs(source, target, env):
    print("Building React App...")
    sys.stdout.flush()

    env.Execute("cd denshi.io && yarn build")
    print("React App built!")
    sys.stdout.flush()

    print("Removing old SPIFFS image...")
    sys.stdout.flush()
    env.Execute("cd .. && rm -rf data")

    print("Copying React App to SPIFFS...")
    sys.stdout.flush()
    env.Execute("cp -r denshi.io/out data")
