# Contributing guide

Thanks for taking the time to contribute!  We appreciate contributions whether
it's:

* Reporting a bug
* Submitting a fix or new feature
* Proposing new features

## Mobile focus

This project emphasizes mobile performance and minimizing binary code size in
order to provide the best experience for mobile app users. Because of our mobile
first focus, please understand that pull requests that do not adhere to this
goal will not be accepted.

## Report bugs using Github [issues](https://github.com/Snapchat/djinni/issues)

We use GitHub issues to track public bugs. Report a bug by opening a new issue;
it's easy!

When reporting bugs, please include enough details so that it can be
investigated. Bug reports should have:

* A summary or background
* Steps to reproduce
* Give code sample if you can
* What is the expected result
* What actually happens

## Contributing fixes and features

Pull requests are the best way to propose code changes:

1. Fork the repo and create your branch from `master`.
2. Run `./examples/run_djinni.sh` and check in any changes to generated code.
3. Add tests, if appropriate.
4. If you add or change code generator command line options, update the
   documentation.
5. Ensure the test suite passes with your change.
6. Test the changes on actual devices if you can.
7. Issue the pull request.

## Code style

Please try to follow the code style in existing files as much as you can when
making contributions.

## License

Any contributions you make will be under the same Apache 2.0 License that covers
the project.
