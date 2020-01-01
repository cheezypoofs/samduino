# The Samduino library: A personal arduino library with google test facilities.

## Motivation

I wanted to get my kids into programming and I was building a project with my son, Sam, who was building a digital thermometer as his first project. As we coded it, I was already wanting badly to be able to library-ize some of the things and be able to do test driven development. This project is an attempt
to consolidate some of the function we wanted (and will want in the future) and give us a way to test the complexities outside of a real environment.

## Layout

All of the code for the acutal library, and the associated unit tests, are found in [lib](#./lib). The files to setup and teardown the test arduino environment are found in [test](#./test). Some [examples](#./examples) are also available to show how some of the things are used.

## Building

This library does not require dependencies for usage, but does require them for building and testing the features on your local workstation.

The local build and test (for your workstation, not for use in sketch) is based on [conan](https://conan.io/) and uses [gtest](https://github.com/google/googletest) to drive the C++ unit tests.

A sample starting flow might look like:

```shell
# Configure one-time to get access to public recipes
$ conan remote add bintray https://api.bintray.com/conan/bincrafters/public-conan

# Install necessary dependencies
$ conan install . -r bintray --build missing

# Compile locally for testing and test
$ conan build .

# It's cheating, but `package` can be used to bundle up the files
# for the actual arduino library. Just zip up all the files in ./package
$ conan package .

```
