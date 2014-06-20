<?php

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

/**
 * Features context.
 */
class FeatureContext extends Drupal\DrupalExtension\Context\DrupalContext
{
  /**
   * Initializes context.
   * Every scenario gets its own context object.
   *
   * @param array $parameters context parameters (set them up through behat.yml)
   */
  public function __construct(array $parameters)
  {
      // Initialize your context here
  }

  /**
   * @Given /^the last user has the ECK permission "([^"]*)" "([^"]*)" "([^"]*)"$/
   */
  public function theLastUserHasTheEckPermission($operation, $object_type, $object_id) {
    dpm("Creating ECK perm: $operation $object_type $object_id \n");
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
