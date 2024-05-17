#!/usr/bin/env python3

##
# @file build.py
# @brief A simple build system
#
# @author Matěj Bucek (matejbucek)


import os
import signal
import sys
import argparse
import tomllib
from typing import Any

project_name = "Build"


##
# @brief A class representing a target in the build system
#
class Target:
    __slots__ = ("name", "dependencies", "action", "description", "rc", "check_for_file")

    ##
    # @brief Constructor
    # @param name The name of the target
    # @param dependencies A list of dependencies
    # @param action The action to run
    # @param description A description of the target
    # @param rc The expected return code
    # @param check_for_file A file to check for existence which determines if the target should run
    def __init__(self, name: str, dependencies: [str], action: str = None, description: str = "", rc: int = 0,
                 check_for_file: str = None):
        ## @brief The name of the target
        self.name = name
        ## @brief A list of dependencies
        self.dependencies = dependencies
        ## @brief The action to run
        self.action = action
        ## @brief A description of the target
        self.description = description
        ## @brief The expected return code
        self.rc = rc
        ## @brief A file to check for existence which determines if the target should run
        self.check_for_file = check_for_file

    ##
    # @brief Run the target
    # @return The return code of the target
    #
    def run(self) -> int:
        if self.check_for_file is not None:
            if os.path.exists(self.check_for_file):
                return self.rc

        if self.action is None:
            return 0
        return os.system(self.action) >> 8

    def __str__(self):
        return f"Target(name: {self.name}, description: {self.description}, dependencies: [{', '.join(self.dependencies)}], expected_rc: {self.rc})"


##
# @brief A class representing the build system
#
class Build:
    __slots__ = "targets"

    ##
    # @brief Constructor
    # @param target_list A list of targets
    #
    def __init__(self, target_list: [Target]):
        ## @brief The list of targets
        self.targets = target_list

    ##
    # @brief Get a target by name
    # @param target The name of the target
    # @return The target
    #
    def get_target(self, target: str) -> Any | None:
        for t in self.targets:
            if t.name == target:
                return t
        return None

    ##
    # @brief Build a dependency graph
    # @param target The target to build the graph from dependencies without duplicates
    # @return The dependency graph
    #
    def build_dependency_graph(self, target: Target) -> [Target]:
        graph = []

        for dep in target.dependencies:
            graph.extend(self.build_dependency_graph(self.get_target(dep)))

        graph.append(target)
        return list(dict.fromkeys(graph))

    ##
    # @brief Run the build system
    # @param cli_targets The targets to run
    # @param debug Enable debug mode
    #
    def run(self, cli_targets: [str], debug: bool = False):
        targets_to_run = []
        for t in cli_targets:
            target = self.get_target(t)
            if target is None:
                print(f"{project_name}: Target %s not found" % t)
                sys.exit(1)
            targets_to_run.extend(self.build_dependency_graph(target))

        if debug:
            print(f'{project_name}: Targets to run:', [str(t) for t in targets_to_run])

        successfully_run = 0

        for t in list(dict.fromkeys(targets_to_run)):
            if debug:
                print(f'{project_name}: Running target', str(t))
            if t.run() == t.rc:
                successfully_run += 1
            else:
                print(f"{project_name}: The build failed on the target {t.name} with rc {t.rc}")
                sys.exit(1)
        print(f"{project_name}: Successfully run {successfully_run} targets")


##
# @brief A function to parse targets from a TOML file
#
def targets_from_toml(toml: dict) -> [Target]:
    project_targets = []
    for target_name, target in toml["targets"].items():
        name = target_name
        dependencies = target.get("dependencies", [])
        action = target.get("action", None)
        description = target.get("description", "")
        rc = target.get("rc", 0)
        check_for_file = target.get("check_for_file", None)
        project_targets.append(Target(name, dependencies, action, description, rc, check_for_file))
    return project_targets


##
# @brief A function to handle SIGINT
#
def sigint_handler(sig, frame):
    sys.exit(1)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, sigint_handler)

    parser = argparse.ArgumentParser(description="RISC-V OS Build system")
    parser.add_argument("-d", "--debug", help="Enable debug mode", action="store_true")
    parser.add_argument("-l", "--list", help="List all available targets", action="store_true")
    parser.add_argument("-p", "--project", help="Project to build", type=str, default="project.toml")
    parser.add_argument("-v", "--version", action="store_true", help="Prints the project version and exits")
    parser.add_argument("targets", type=str, nargs="*", help="Targets to build")

    args = parser.parse_args()

    if not os.path.exists(args.project):
        print(f"{project_name}: Project file not found")
        sys.exit(1)

    targets = []
    with open(args.project, "rb") as f:
        try:
            project = tomllib.load(f)
            targets = targets_from_toml(project)
        except tomllib.TOMLDecodeError as e:
            print(f"{project_name}: Error decoding project file:", e)
            sys.exit(1)

    if args.version:
        print(f"{project.get("name", "NAME NOT SPECIFIED")}: {project.get("version", "No version found")}")
        sys.exit(0)

    if len(targets) == 0:
        print(f"{project_name}: No targets found in project file")
        sys.exit(1)

    build = Build(targets)

    if args.list:
        print("Available targets:")
        for t in targets:
            print(t)
        sys.exit(0)

    if len(args.targets) > 0:
        build.run(args.targets, args.debug)
    else:
        build.run(["all"], args.debug)
