namespace SQLamarr
{
  /** A collection of initializers specializing SQLamarr classes to tasks.
   
    The namespace `SQLamarr::BlockLib` groups functions defining specialized blocks
    making assumptions on the workflow in which these blocks will be deployed.
    While useful for testing and for organizing the code defining pipelines, 
    it is not supposed to be stable (as it is being modified while the workflow
    under test is modified) and should not be used as part of other packages. 

    Other packages, however, may take inspiration from the structure of `BlockLib`
    to design specialized blocks, resident in their codebase, in a more 
    organized way than having everything pipelined in a single file.

    To test the completeness of the feature set in the main part of the library,
    `BlockLib` is designed to only include functions, accessing public methods 
    of the objects defined in the main part of the library.
  */
  namespace BlockLib
  {
  }
}
