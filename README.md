
GEPARD [![Build Status](https://travis-ci.org/GepardGraphics/gepard.svg?branch=master)](https://travis-ci.org/GepardGraphics/gepard) [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/GepardGraphics/gepard?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
=====

The Gepard aims to be a hardware accelerated 2D
rendering engine.

Aside from the main goal (that is hardware accelerated) we aim to
create an engine which is:
* lightweight: as there should be only rendering related part in the code.
* modular/configurability: provide configuration options where the developer
can specify which part is needed.

## How to build

You need to install some dependencies, after use the
```
make
```
command for release build.

## For developers

You need to install some dependencies for contributing. After please use the scripts in `./tools/scripts` directory for building, testing and checking your patches.

### Our contributing process is:

* [Fork](https://github.com/GepardGraphics/gepard#fork-destination-box) the _gepard_ repository. (Only the first time.)

2. Create a branch like these: `fix-min-compute`, `15-canvas2d-path`, `update-readme_szledan`, `update-readme_bzsolt`, etc.

    Note: The number at the beggin of second branch name means the isse _id_. _Nicknames_ at the two last examples differentiate the same works.

3. Create your patch. Please use `sign-off` every commits. Git usage: `git commit -s|--sign-off` (https://git-scm.com/docs/git-commit).

4. If you are ready, create a _Pull request_(PR) in the `GepardGraphics/gepard/master`.

5. If you are know how is your specialist in rivewers then assign him/her, otherwise ignore it.

6. You need 2 `lgtm`s (Looks Good To Me), `+1` or `r+` for merge your patch in to the _gepard_.

See more: __For contributors__ section in [wiki](https://github.com/GepardGraphics/gepard/wiki) page.

