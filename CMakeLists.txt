cmake_minimum_required(VERSION 3.18)

project(BGCMSProject)

#add_subdirectory(CallGraph)
#add_subdirectory(BGMRPCClient)

add_subdirectory(BGCMS)
add_subdirectory(CMSPlugins/CMSContentEditor)
add_subdirectory(CMSPlugins/RPCLog)
add_subdirectory(CMSPlugins/CMSFilter)
add_subdirectory(CMSPlugins/CMSMedia)
add_subdirectory(CMSPlugins/CMSResourceEditor)
add_subdirectory(CMSPlugins/BrowserHeaderView)
add_subdirectory(CMSPlugins/CMSNodeInfo)
add_subdirectory(CMSPlugins/CMSPathHistory)
