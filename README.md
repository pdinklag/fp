# fp &ndash; Rabin-Karp Fingerprinting with Mersenne primes for C++20

(A description of this repository is to follow)

## License

```
MIT License

Patrick Dinklage, Jonas Ellert and Alexander Herlez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Usage

The library is header only, so all you need to do is make sure it's in your include path.

In case you use CMake, you can embed this repository into yours (e.g., as a git submodule) and add it like so:

```cmake
add_subdirectory(path/to/fp)
```

You can then link against the `fp` interface library, which will automatically add the include directory to your target.

## Fingerprinting with Mersenne primes

For a string $S \in \Sigma^\ell$ of length $\ell$, we compute the Rabin-Karp fingerprint $\phi(S)$ using the well-known formula $\phi(S) = \left(\sum_{i=1}^{\ell} S[i] \cdot \sigma^{\ell-1-i} \right) \bmod q$, where $\sigma$ is the *base* and $q$ prime. Fixing $q$ to a Mersenne prime allows us to harness huge performance benefits regarding the modulo operation.

Let $q = 2^a-1$ be a Mersenne prime, then for any $x < 2^{2a}-1$, it holds that $x \bmod q = (x \gg a) + (x \land q) - bq$, where $b \in \{0,1\}$ is 1 iff $(x \gg a) + (x \land q) < q$. Thus, the modulo operation against $q$ is reduced to very simple arithmetics (modern compilers will optimize the multiplication $bq$ to a conditional move since $b$​ is a Boolean, thus avoiding a branch).

This library features Karp-Rabin fingerprinting for 32-bit and 64-bit fingerprints using the Mersenne primes $2^{31}-1$ and $2^{61}-1$, respectively. If we store $x$ in a correspondingly wide integer (i.e., 32 or 64 bits, respectively), we guarantee the above condition that $x < 2^{2a}-1$.

### The Power-of-Two Base Pitfall

When using Mersenne primes for fingerprinting ASCII-encoded text, choosing $\sigma$ to be a power of two causes a clustering of fingerprint collisions. The root cause is found in the nature of how modular arithmetics behave with Mersenne primes versus how ASCII is defined regarding capital and small letters. It cannot be resolved other than choosing a different base. The library will emit a warning to the standard error stream in case the chosen base happens to be a power of two.
