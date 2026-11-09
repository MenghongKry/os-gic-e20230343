# Lab 2 — Linux Navigation and File Management: individual report

Copy this template to `lab2/README.md` in your own course repository. Follow [the instructions](lab2-instruction.md). Your prediction, debrief and checkpoint answers are already saved by `oslab`; do not copy them here. The marks come from those records and your evidence files; this report is your own record of the lab.

- Student name/ID: Kry Menghong / e20230343
- Ubuntu username: gic-kry-menghong
- My values (`oslab values lab2`): file1 = cash flow.txt, file2 = tax summary.txt, owner = Legal, reader = Support, third = HR
- Files: `evidence/paths.txt`, `evidence/tree.txt`, `evidence/checkpoint.txt`

## What I Did

| Step | My command | Current folder when I ran it |
|---|---|---|
| Move the first file (relative destination) | `mv -- "cash flow.txt" ../reports/` | `~/oslab-work/lab2/incoming` |
| Move the second file (absolute paths) | `mv -- "/home/gic-kry-menghong/oslab-work/lab2/incoming/tax summary.txt" "/home/gic-kry-menghong/oslab-work/lab2/reports/"` | `~/oslab-work/lab2` |
| Read the report from my reader department (relative path) | `cat "../Legal/cash flow.txt"` | `~/oslab-work/lab2/TechCorp/Support` |
| The path error I made on purpose, and its message | `cat "reports/cash flow.txt"` → `cat: 'reports/cash flow.txt': No such file or directory` | `~/oslab-work/lab2/TechCorp/Support` |
| The relative path that fixed it | `../../reports/cash flow.txt` | `~/oslab-work/lab2/TechCorp/Support` |

## Explanation

A correct file name can still give "No such file or directory" because a relative path always starts from the current folder, not from the lab folder or my home. `reports/cash flow.txt` was right from `lab2`, but from `TechCorp/Support` the shell looked for `TechCorp/Support/reports/`, which does not exist; going up two levels with `../../` fixed it. `mv` moves a file, so after `mv -- "cash flow.txt" ../reports/` the file was in `reports/` and `incoming/` was empty. `cp` makes a second file and keeps the original, so after copying, `cash flow.txt` was in both `reports/` and `TechCorp/Legal/`.

## What My Tests Do Not Prove

`ls` and `find` only show that files with the right names are in the right folders, not that their contents are correct. `cmp` printing nothing proves that each copy in `TechCorp/Legal/` is byte-for-byte the same as the file in `reports/`, but not that the file in `reports/` is still the same as the original that arrived in `incoming/` (I only checked that by reading it with `cat`).

## Plus / Challenge

<!-- Delete this section if you did not do Plus. -->
Plus: from `mess/c` I moved and renamed the lost file with one command: `mv -- '../a/b/lost file.txt' found.txt`. From `mess/a/b` I copied it with `cp -- ../../c/found.txt ../../../TechCorp/HR/`. I needed three `..` to get from `mess/a/b` back to the lab folder (b → a → mess → lab2), and two to reach `found.txt` in `mess/c`.
