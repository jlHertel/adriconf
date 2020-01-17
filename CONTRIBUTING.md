# Contributing to adriconf

Our project is always open to new contributions.
Be it a code change, a bug report on even an idea, get in touch with us and we
can try to help.

## Reporting issues

Reporting issues is a good way to contribute to the project. We always 
appreciate a well-written, thorough bug reports.

Prior to raising a new issue, check out our issue list to determine whether it 
already include the problem you are facing.

A good bug report shouldn't leave others needing to chase you up for more 
information. Please try to be as detailed as possible. 
The following questions might help us better understand the issue:

- What were you trying to achieve?
- What brand and models of GPUs you have?
- How many monitors you have?
- Are you running X11? Maybe Wayland?
- Are you using the latest mesa version?
- Is your distribution and adriconf updated?
- What are the expected results?
- What are the received results?
- What are the steps to reproduce the issue?

## Enabling debug mode

Since version 1.5 we support different log levels, which gives a lot more output
and helps in finding issues.
The log levels are the following:

- ERROR: Only display errors
- WARNING: Display errors and warnings
- INFO: Will display general messages, as well as WARNING and ERROR. Is the 
default mode.
- DEBUG: Is the most verbose mode. Will output all possible messages.

To enable it you can run the application with the environment variable
`VERBOSITY` to the desired level. Example: `VERBOSITY=DEBUG ./adriconf`

## Submitting Patches

To submit a patch, we kindly ask you to use the Pull Request functionality.
Fork the repository, make your changes and then create the Pull Request.

Your Pull Request will only be accepted if the units tests are passing, so test
your changes before submitting it.

## Review Proccess

When you submit a Pull Request the code will be checked against some criteria.

- The code should try to follow the same code style of other code.
- The code should not introduce new bugs.
- Whenever possible, the code should also have a unit test.
- Current unit tests are passing.
- The code is compiling and working with a recent Linux distribution.

## Release Proccess

New releases are made sporadically, usually every 3 to 4 months.
Releases can also be made in case of a high number of users affected by an 
issue like some specific linux distribution not working anymore, or the 
last release has broken something.

To view instructions on how to release check our [RELEASING](RELEASING.md).