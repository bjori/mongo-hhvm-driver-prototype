--TEST--
MongoDB\Driver\Manager - no connection (command)
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:44444/?serverselectiontimeoutms=500");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );

try {
	$m->executeCommand( 'demo', $c );
	echo "Expected exception not thrown\n";
}
catch ( MongoDB\Driver\Exception\ConnectionTimeoutException $e )
{
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
No suitable servers found
