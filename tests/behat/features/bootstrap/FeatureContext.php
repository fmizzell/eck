<?php
/**
 * @file
 * Features context.
 */

use Behat\Behat\Context\ClosuredContextInterface,
    Behat\Behat\Context\TranslatedContextInterface,
    Behat\Behat\Context\BehatContext,
    Behat\Behat\Exception\PendingException;
use Behat\Gherkin\Node\PyStringNode,
    Behat\Gherkin\Node\TableNode;

//
// Require 3rd-party libraries here:
//
//   require_once 'PHPUnit/Autoload.php';
//   require_once 'PHPUnit/Framework/Assert/Functions.php';
//

// @codingStandardsIgnoreStart
class FeatureContext extends Drupal\DrupalExtension\Context\DrupalContext {
  /**
   * {@inheritdoc}
   */
  public function __construct(array $parameters) {
  }

  /**
   * @Given /^the last user has the ECK permission "([^"]*)" "([^"]*)" "([^"]*)"$/
   */
  public function theLastUserHasTheEckPermission($operation, $object_type, $object_id) {
    // print_r("Creating ECK perm: $operation $object_type $object_id \n");
    $user = $this->user;
    try {
      $perm = new ECKPermission();
    }
    catch(Exception $e) {
      Throw new Exception("ECK Permissions is not installed");
    }
    $perm->type = "user";
    $perm->oid = $user->uid;
    $perm->permission = "$operation $object_type:$object_id";

    $perm->save();
  }
}
// @codingStandardsIgnoreEnd