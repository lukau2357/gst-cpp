Implementation of the generalized suffix tree data structure in C++, along with modules for testing the correctness of the implementation. The tree is hardcoded to support 8 input strings (8 different terminating characters for each input strings), however this is easily extendable and should be a problem in the domain of bioinformatics where the alphabet size is 5 (considering nitrogenous bases of both DNA and RNA molecules). The following queries are implemented:

- $substring(S, T)$ - check if $S$ is a substring of at least one sequence in the tree $T$ .
- isSuffix(S, T) - check if S is a substring of at least one sequence in the tree T.
- $occurrences(S, T)$ - Compute occurrences of $S$ accross all sequences in the tree $T$
- $lcs(T)$ - Compute the longest common substring of all tree sequences in linear time.
- $generalizedSubstring(minLength, minString, minOccurrences, T)$ - The most general query, return all substrings of the tree $T$ that are of minimum length $minLength$, occurr in minimum $minString$ sequences of the tree $T$, and in each of them they occurr at least $minOccurrences$ times. 