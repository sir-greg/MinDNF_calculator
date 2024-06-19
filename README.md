# MinDNF_calculator
This program finds all minimal DNFs of a logical function with N variables and which has the number NUM.

The calculation goes through a 6-step process:
1. A map with all different conjunctions is drawn (the first column is the values of the function)
2. All rows whose value in the first column is equal to 0 are crossed out.
3. For every column, if a number was crossed out in (2), than it will be crossed out in (3) in the whole column.
4. Here we do an "absorption":
- Consider two conjunctions of variables (We will call these conjunctions A and B. A and B are NOT crossed out)
- If all variables of A are presented in B, and have the same boolean value, i.e. A is a subset of B, then B is crossed out
- The followind procedure is done for every pair A and B.
5. For every row, we brute force through all combinations of conjunctions we can take. From these combinations we take the minimal DNFS of the function.

This project was done using SFML.
MinDNFwithExecutable.zip has a working exe file
