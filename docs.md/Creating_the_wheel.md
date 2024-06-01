# Distributing as a binary wheel

To compile and distribute the wheel of `SQLamarr` in PyPI, you will need to 
compile it with an older tool chain in order to guarantee compatibility on 
older Linux distributions. 

Indeed, PyPI forbids uploading wheel that do not specify a compatibility tag
used to choose the right wheel when running `pip install SQLamarr`.

## Using docker 
To compile the wheel in compatibility mode, we are using the following recipe,
which assumes the working directory is the root of the git repository
```bash
docker run -it -v $PWD:/mylib quay.io/pypa/manylinux2014_x86_64:latest \
         bash /mylib/make_wheel.sh
python3 -m twine upload wheelhouse/*.whl
```

## Using Apptainer
In some environments, docker is not available and apptainer should be used instead.

First convert the docker image to an SIF image (this is only required once). 
```bash
apptainer build /tmp/pypa.sif docker://quay.io/pypa/manylinux2014_x86_64:latest
```

Then execute the following script (as for the docker case, it is assumed 
the working directory is the root of the git repository).

```build
apptainer exec --bind $PWD:/mylib --writable-tmpfs /tmp/pypa.sif /bin/bash make_wheel.sh
```

## Additional information:
 * Distributing binaries in compatibility mode: https://github.com/pypa/manylinux
 * Example using travis for CI/CD: https://github.com/pypa/python-manylinux-demo



