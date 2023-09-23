# urxvt_alpha
`urxvt_alpha` is a program to change the transparency of the `urxvt` terminal
and print the current value of alpha to `stdout`. 

## Synopsis
```
urxvt_alpha [-+]
```

## Description
To change the transparency, `urxvt_alpha` must read and write the corresponding file. Please note that you must have the right permissions to use this program.

## Usage
- `-`: Decrease screen urxvt_alphaness, and then print to `stdout`.
- `+`: Increase screen urxvt_alphaness, and then print to `stdout`.
- Without arguments, just print current urxvt alpha.

## Configuration
Edit `urxvt_alpha.c` and recompile.

## Author
Lucas Mior   <lucas.mior at tutamail.com>
