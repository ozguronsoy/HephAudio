## How to contribute to HephAudio

### Reporting bugs

- Ensure the bug was not already reported by searching the [issues](https://github.com/ozguronsoy/HephAudio/issues).
- Open a new issue with a clear title and description. You can check the [bug report template](/.github/ISSUE_TEMPLATE/bug_report.md) for an example.


### Fixing bugs

- Create a new pull request with the patch.
- The PR should clearly describe the problems and the solutions.
- Patch must support C++ 17 and above.
- Ensure the patch works with all internal sample types if applicable.
- Ensure the patch works for all of the supported platforms.
- Ensure the patch works with both FFmpeg enabled and disabled if applicable.


### Adding new features or changing the existing ones

- Open a new issue and describe the features you want to add/change. You can check the [feature request template](/.github/ISSUE_TEMPLATE/feature_request.md) for an example.
- Write the code and create a pull request.
- Same conditions mentioned in ``Fixing bugs`` also applies.


### Writing code

- Name of the variables, methods, classes, enums, and macros must be clear. One should be able to understand what it is/does simply by looking at the name. If not write a comment on top of the decleration.
- Name of the macros must be all capital separated with underscore ``_``. Ex: ``MY_MACRO_NAME``.
- Variable names should be in camel case. You can append units with underscore if necessary. Ex: ``phaseAngle_rad``.
- Class and method names should be in pascal case. Ex: ``MyMethodName``.
- No in-code documentation is required as long as the code is clear. But can be added if desired.
