GEPARD [![Build Status](https://travis-ci.org/GepardGraphics/gepard.svg?branch=master)](https://travis-ci.org/GepardGraphics/gepard) [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/GepardGraphics/gepard?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
=====

The Gepard aims to be a hardware accelerated 2D
rendering engine.

Aside from the main goal (that is hardware accelerated) we aim to
create an engine which is:
* lightweight: as there should be only rendering related part in the code.
* modular/configurability: provide configuration options where the developer
can specify which part is needed.

## Getting Started

Check out the source code
```
git clone https://github.com/GepardGraphics/gepard.git
```

Install any necessary dependencies
```
./tools/install-deps.sh
```

Build `gepard` dynamic library
```
./tools/build.py
```

Note: To list all available options, run with the `-h` flag.

## Build & run examples

Build all examples in `./apps/examples`
```
./tools/build.py examples
```

Run a GLES2 example (now the _fill-rect_)
```
./build/gles2/bin/fill-rect
```

## For developers

Contribution to the project is done by using the fork model. ([GitHub help](https://help.github.com/articles/working-with-forks/))

After you _fork_ and _clone_ the Gepard repository, you will need to install more dependencies for contributing.
```
./tools/install-deps.sh --developer
```

If you are looking for easy tasks, please check the [low hanging fruits](https://github.com/GepardGraphics/gepard/issues/48) (LHF).
Otherwise, feel free to browse other [issues](https://github.com/GepardGraphics/gepard/issues) as well.

#### Our contributing process is:

1. Create a branch with a name like: `fix-min-compute`, `15-canvas2d-path`, etc.

    ```
    git checkout -b my-first-patch
    ```
    
    Note: About the branch naming in Gepard. If you work on an [issue](https://github.com/GepardGraphics/gepard/issues),  then it is recommended to start your branchname with the issue number.

2. Create your patch.
    
    Note: Please use `sign-off` in every commits. Git usage: `git commit -s|--sign-off` (https://git-scm.com/docs/git-commit).

4. If you are ready, create a _Pull request_ (PR) to the `GepardGraphics/gepard/master`. (If you know a reviewer who works on that topic, then you should assign her/him, otherwise leave it unassigned.)

5. Your PR will be approved if you get 2 __lgtm__-s from the [reviewers](https://github.com/orgs/GepardGraphics/teams/gepard-reviewers).

For more information see the [wiki](https://github.com/GepardGraphics/gepard/wiki) page's '__For contributors__' section.
