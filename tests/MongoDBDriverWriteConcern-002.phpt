--TEST--
MongoDB\Driver\WriteConcern debug output should include all fields for w default
--FILE--
<?php
/* -2 is libmongoc's constant for relying on the server's default value for "w".
 * Although "w" will be omitted from the write concern sent to the server, we
 * should still yield other fields in the debug output, which may be sent.
 */
$w = new MongoDB\Driver\WriteConcern(-2, 1000, true, true);

var_dump($w);

?>
--EXPECTF--
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(1000)
  ["fsync"]=>
  bool(true)
  ["journal"]=>
  bool(true)
}
