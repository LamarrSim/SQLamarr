class PyTransformer:
  def __init__(self, db):
    self._db = db

  def __call__(self, f):
    def wrapped():
      with self._db.connect() as c:
        f (c)
    return wrapped

