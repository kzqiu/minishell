# Homework 6

In this homework, you will write a minishell that emulates bash.

Instructions can be found under 

    instructions.md

All your changes must be done under the `src` directory. You can create it using `mkdir`. 
A sample executable is provided in the test directory.

Your code should be organized as follows. (Ignoring all markdown files)

    src
        \_ Makefile
        \_ minishell.c
    test
        \_ minishell

All files and directories must be named exactly as above case-sensitive. 
You should not commit any extraneous files, such as object files or executables (besides the given one in the test directory).

All rules about Makefiles and compilation are listed on HW2.

If you are allocating heap memory, you should follow all rules listed on HW3 and HW4. 

All submissions must include at least five git commits with **meaningful** messages.

## Submission

To submit:

    git commit -am "hw6 completed"
    git tag -a "handin" -m "hw6 submission"
    git push origin main
    git push origin handin

After submitting, you should re-test with your submitted version. Details on how to do that are [here](https://github.com/cs3157-borowski/guides/blob/main/submission.md).
