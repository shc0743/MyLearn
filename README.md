# MyLearn

My Learning Projects

## About `../../resource/tool.h`

Many of my projects use the `../../resource/tool.h` file. However, this file may not be included in some repositories, which could cause confusion. To clarify, `tool.h` is a collection of utility functions. You can find the source code in the [resource/](https://github.com/shc0743/MyLearn/tree/main/resource) directory.

## About Code Signing

As an in-board student, I cannot afford an official code-signing certificate. To provide some level of identification, I sometimes sign binary files using a self-signed certificate. The fingerprint of the root certificate(s) is as follows:

```
50da2532174f6430287a6a6ca450e90371e0e5db    CN = chcs   sha1RSA     ‎2035‎年‎10‎月‎13‎日 0:00:00    (**deprecated**, will no longer be used in new projects)
243e10237e27e73c71366fdf8d957a8470728a2b    CN = chcs Root Certificate Authority X1     sha384ECDSA     ‎2038‎年‎12‎月‎18‎日 12:18:01
```

The fingerprint of the middle certificate(s) is as follows:

```
a443f1f36a04cb6017b0aba903cfeaf5fdf518e7    CN = chcs Trusted Root G1   sha384ECDSA     ‎2035‎年‎10‎月‎13‎日 12:24:01
2900bc489947fef80ede942dac26915032539a0d    CN = chcs Trusted G1 Code Signing ECC384 SHA384 2025 CA1    sha384RSA       ‎2030‎年‎10‎月‎13‎日 12:24:01
```

The fingerprint of the end-entity certificate(s) is as follows:

[3529cdfcde4354c0b609d23ada41e07eccecb511](./chcs1013.cer)    `CN = chcs1013, C = CN, OU = GitHub, O = chcs1013@github`     `sha384ECDSA`     `‎2027‎年‎3‎月‎30‎日 12:24:02` [full chain file](./chcs1013.fullchain.cer)

The certificates can be found in [.crt](./.crt/).

# License

The general license for these projects is [LGPL-3.0](./LICENSE.md). However, some projects may use different licenses as specified in their respective repositories.
