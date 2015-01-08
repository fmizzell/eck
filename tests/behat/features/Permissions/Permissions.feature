@api @permissions
Feature: Permissions
  As a site administrator
  I want to control who can and can not manage eck permissions
  so someone doesn't mess them up

  @eck-perm
  Scenario: Users without the right permission can't access eck's permission page
    Given I am logged in as a user with the "Administer permissions,Use the administration pages and help,Administer users" permissions
    And I visit "admin/people"
    And I should not see the link "ECK Permissions"

  @eck-perm
  Scenario: Users with the right permission can access eck's permission page
    Given I am logged in as a user with the "Administer ECK Permissions" permissions
    And I visit "admin/people/eck-permissions"
    Then I should get a "200" HTTP response
