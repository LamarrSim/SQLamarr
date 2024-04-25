# docker run -it -v $PWD:/mylib quay.io/pypa/manylinux2014_x86_64:latest bash
export PYBIN=/opt/python/cp37-cp37m/bin/python3
yum check-updates 
yum install -y HepMC3 HepMC3-devel

cd /mylib;
rm -f wheelhouse/*.whl | echo "Ok";
rm -f dist/*.whl | echo "Ok";

for PYVERSION in cp312-cp312 cp311-cp311 cp310-cp310 cp37-cp37m cp38-cp38 cp39-cp39;
do
  export PYBIN=/opt/python/$PYVERSION/bin/python3
  echo "Preparing binary distribution with $PYBIN";
  $PYBIN clean --all;
  $PYBIN setup.py bdist_wheel;
done

for WHEEL in `find dist/*.whl`;
do 
  auditwheel repair $WHEEL;
done

