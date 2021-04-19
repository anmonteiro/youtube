001-reason-vercel-lambda-function
----

This is an example of deploying a lambda function written in
[Reason](https://reasonml.github.io) to [Vercel](https://vercel.com).

It uses the `lambda-runtime` and `vercel` libraries, making it seem like a
typical request / response exchange.

## Installing

To install the dependencies required to build the project, go get [esy](esy)
first. Most workflows can just `npm install -g esy`.

[esy]: https://esy.sh

The necessary dependencies can then be installed by installing `esy`

## Building

This project is built using [`dune`](dune) To build the project, run `esy b
dune build @all`.

[dune]: https://dune.build

## Deploying to `vercel`

Deploying OCaml code to Vercel is the more complicated part here. There are a
few things going on:

- Vercel runs on AWS Lambda, which in turn are running on Amazon Linux boxes
- We use e.g. a dependency on OpenSSL. In order to avoid depending on runtime
  packages, we just build a statically linked binary.

### Linux users

- Run `esy b dune build src/doglambda.exe --profile=static` to build a
  statically linked executable. Look in the root [`dune`](./dune) for the profile
  definition (basically passing `-ccopt -static` to `ocamlopt`).
- Run `cp _build/default/src/doglambda.exe ./bootstrap` (our
  [`vercel.json`](./vercel.json)) expects an executable named `bootstrap`.

### Other OSes

- If you're not on Linux, then you'll need e.g. Docker to build the project. If
  this is the case, I've provided a [`build.sh`](./build.sh) script that calls
  Docker for you, and copies the binary from the Docker image to the path that
  Vercel expects.
