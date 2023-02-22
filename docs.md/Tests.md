# Test infrastructure

Unit testing relies on the [`pytest`](https://docs.pytest.org) package. 

To run all the tests,
```bash
pip install pytest
pytest --durations=0 -v
```

## No-printout test
`test/test_no_printout.py` tests for no printout in C, C++ and Python code,
including the `setup.py` script.
