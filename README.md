# RTSyn Comedi Device

COMEDI data acquisition device for analog and digital hardware I/O.

This template implements a generic RTSyn device module. Device modules are called by the runtime at
the beginning and at the end of each cycle, so generated process code is split between immediate and
terminal stage hooks.

## Usage

### Update

Make sure you have last version of the dependencies:

```bash
xrepo update-repo
xmake require --upgrade
```

For development you may need to run:

```bash
xmake require --upgrade -fy <dependency_name>
```

### Compiling

```bash
xmake
```

### Tests

```bash
xmake test
```

For enabling valgrind, before running tests:

```bash
xmake f --valgrind=y
```

### Local development

If you want to test your changes locally from different parts of RTSyn, export the path where you have all the repos:

```bash
export RTSYN_WORKSPACE=<PATH>
```

> [!WARNING]
> This expects you also the `rtsyn-xmake-repo`.
