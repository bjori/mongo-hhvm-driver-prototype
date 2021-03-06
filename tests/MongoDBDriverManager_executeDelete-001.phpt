--TEST--
MongoDB\Driver\Manager::executeDelete
--FILE--
<?php
include 'utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

/* Inserting some data */
$m->executeInsert( 'demo.test', [ 'd' => 2 ] );
$m->executeInsert( 'demo.test', [ 'd' => 3 ] );
$m->executeInsert( 'demo.test', [ 'd' => 4 ] );
$m->executeInsert( 'demo.test', [ 'd' => 5 ] );
$m->executeInsert( 'demo.test', [ 'd' => 6 ] );
$m->executeInsert( 'demo.test', [ 'd' => 7 ] );
$m->executeInsert( 'demo.test', [ 'd' => 8 ] );
$m->executeInsert( 'demo.test', [ 'd' => 9 ] );

// delete one
show_obj_properties( $m->executeDelete( 'demo.test', [ 'd' => [ '$gte' => 2, '$lte' => 3 ] ], [ 'limit' => true ] ), [ 'deleted', 'matched' ] );

// multi delete
show_obj_properties( $m->executeDelete( 'demo.test', [ 'd' => [ '$gte' => 4, '$lte' => 5 ] ], [ 'limit' => false ] ), [ 'deleted', 'matched' ] );
show_obj_properties( $m->executeDelete( 'demo.test', [ 'd' => [ '$gte' => 7, '$lte' => 8 ] ] ), [ 'deleted', 'matched' ] );

var_dump( $m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
deleted: 1; matched: 0; 
deleted: 2; matched: 0; 
deleted: 2; matched: 0; 
array(3) {
  [0]=>
  object(stdClass)#%d (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (1) {
      ["oid"]=>
      string(24) "%s"
    }
    ["d"]=>
    int(3)
  }
  [1]=>
  object(stdClass)#%d (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (1) {
      ["oid"]=>
      string(24) "%s"
    }
    ["d"]=>
    int(6)
  }
  [2]=>
  object(stdClass)#%d (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (1) {
      ["oid"]=>
      string(24) "%s"
    }
    ["d"]=>
    int(9)
  }
}
