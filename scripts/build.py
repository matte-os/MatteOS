#!/usr/bin/env python3
import os
import signal
import sys
import argparse
import tomllib


class Target:
    __slots__ = ("name", "dependencies", "action", "description", "rc")

    def __init__(self, name: str, dependencies: [str], action: str = None, description: str = "", rc: int = 0):
        self.name = name
        self.dependencies = dependencies
        self.action = action
        self.description = description
        self.rc = rc

    def run(self) -> int:
        if self.action is None:
            return 0
        return os.system(self.action) >> 8

    def __str__(self):
        return f"Target(name: {self.name}, description: {self.description}, dependencies: [{', '.join(self.dependencies)}], expected_rc: {self.rc})"


class Build:
    __slots__ = "targets"

    def __init__(self, target_list: [Target]):
        self.targets = target_list

    def get_target(self, target: str) -> Target:
        for t in self.targets:
            if t.name == target:
                return t

    def build_dependency_graph(self, target: Target) -> [Target]:
        graph = []

        for dep in target.dependencies:
            graph.extend(self.build_dependency_graph(self.get_target(dep)))

        graph.append(target)
        return list(dict.fromkeys(graph))

    def run(self, cli_targets: [str], debug: bool = False):
        targets_to_run = []
        for t in cli_targets:
            target = self.get_target(t)
            if target is None:
                print("Target %s not found" % t)
                sys.exit(1)
            targets_to_run.extend(self.build_dependency_graph(target))

        if debug:
            print('Debug: Targets to run:', [str(t) for t in targets_to_run])

        successfully_run = 0

        for t in list(dict.fromkeys(targets_to_run)):
            if debug:
                print('Debug: Running target', str(t))
            if t.run() == t.rc:
                successfully_run += 1
        print(f"{len(targets_to_run)} targets run, {successfully_run} successfully run, {len(targets_to_run) - successfully_run} failed")


def targets_from_toml(toml: dict) -> [Target]:
    project_targets = []
    for target_name, target in toml["targets"].items():
        name = target_name
        dependencies = target.get("dependencies", [])
        action = target.get("action", None)
        description = target.get("description", "")
        rc = target.get("rc", 0)
        project_targets.append(Target(name, dependencies, action, description, rc))
    return project_targets


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
        print("Project file not found")
        sys.exit(1)

    targets = []
    with open(args.project, "rb") as f:
        try:
            project = tomllib.load(f)
            targets = targets_from_toml(project)
        except tomllib.TOMLDecodeError as e:
            print("Error decoding project file:", e)
            sys.exit(1)

    if args.version:
        print(f"{project.get("name", "NAME NOT SPECIFIED")}: {project.get("version", "No version found")}")
        sys.exit(0)

    if len(targets) == 0:
        print("No targets found in project file")
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
