import json, pathlib, unittest
ROOT=pathlib.Path(__file__).resolve().parent
class ErrorCatalogTests(unittest.TestCase):
 def test_catalog_shape(self):
  rows=json.loads((ROOT/"catalog.json").read_text(encoding="utf-8"))
  self.assertGreaterEqual(len(rows),25)
  self.assertEqual(len({r["id"] for r in rows}),len(rows))
  for r in rows:
   for key in ("category","title","pattern","cause","steps","sample"):
    self.assertTrue(r[key])
 def test_html_and_scanner(self):
  self.assertIn("catalog.json",(ROOT/"index.html").read_text(encoding="utf-8"))
  self.assertIn("Wire.endTransmission",(ROOT/"I2C_Scanner"/"I2C_Scanner.ino").read_text(encoding="utf-8"))
if __name__=="__main__": unittest.main()
