# Release documentation

## Update application version

In the main CMakeLists.txt file update the APP_VERSION value to the new version

```
...
# define application version here
add_definitions(-DAPP_VERSION="v1.0.0")
...
```

## Update [CHANGELOG.md](./CHANGELOG.md) file

Write in this file everything related to the new version, and the list of changelogs. See the next example as template:

```markdown
# Version 1.0.0

<Concise description highlighting the main features/changes of the version.>

## Change Log

- Added: <description>
- Changed: <description>
- Fixed: <description>
```

## Tag creation

> The release pipeline will be only trigger by the version tag.

1. Create new tag version: `git tag v1.0.0`
2. Push tag to github: `git push origin v1.0.0`
