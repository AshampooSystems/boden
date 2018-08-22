pipeline {
    agent none

    stages {
        stage('Platforms') {
            parallel {
                stage('Linux') {
                    stages {
                        stage('Build') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_linux'
                                    additionalBuildArgs '-t boden_linux'
                                    args '--volume ${WORKSPACE}:/boden -w /boden'
                                }
                            }
                            steps {
                                sh 'python build.py build --platform linux --build-system make --config Release $BUILD_EXTRA_ARGS'
                                stash includes: 'build/**/*', name: 'boden_linux_builddir'
                            }
                        }
                    
                        stage('Test') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_linux'
                                    additionalBuildArgs '-t boden_linux'
                                    args '--volume ${WORKSPACE}:/boden -w /boden'
                                }
                            }
                            steps {
                                unstash 'boden_linux_builddir'

                                sh 'mkdir -p testresults'

                                sh 'python build.py --platform linux --config Release --module testboden -- run --out testresults/linux_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform linux --config Release --module testbodenui -- run --out testresults/linux_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('Android') {
                    stages {
                        stage('Build') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_android'
                                    additionalBuildArgs  '-t boden_android'
                                    args '--volume ${WORKSPACE}:/boden -w /boden'
                                }
                            }

                            steps {
                                sh 'python build.py prepare --platform android --arch x86_64 --build-system AndroidStudio'
                                sh 'python build.py build --platform android --arch x86_64 --build-system AndroidStudio --config Release'
                                stash includes: 'build/**/*', name: 'boden_android_builddir'
                            }
                        }
                    }
                }
                stage('MacOS') {
                    stages {
                        stage('Build') {
                            agent { label 'macOS' }
                            steps {
                                sh 'python build.py prepare --platform mac --build-system Xcode'
                                sh 'python build.py build --platform mac --build-system Xcode --config Release --module testboden'
                                stash includes: 'build/**/*', name: 'boden_mac_builddir'
                            }
                        }
                        stage('Test') {
                            agent { label 'macOS' }
                            steps {
                                unstash 'boden_mac_builddir'

                                sh 'mkdir -p testresults'
                                
                                sh 'python build.py --platform mac --build-system Xcode --config Release --module testboden --run-output-file testresults/mac_testboden.xml -- run --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testboden.xml"

                                sh 'python build.py --platform mac --build-system Xcode --config Release --module testbodenui --run-output-file testresults/mac_testbodenui.xml -- run --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('IOS') {
                    stages {
                        stage('Build') {
                            agent { label 'macOS' }
                            steps {
                                sh 'python build.py prepare --platform ios --build-system Xcode'
                                sh 'python build.py build --platform ios --build-system Xcode --config Release'
                                stash includes: 'build/**/*', name: 'boden_ios_builddir'
                            }
                        }

                        stage('Test') {
                            agent { label 'macOS' }
                            steps {
                                unstash 'boden_ios_builddir'

                                sh 'mkdir -p testresults'

                                sh 'python build.py --platform ios --build-system Xcode --config Release --module testboden --run-output-file testresults/ios_testboden.xml -- run --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testboden.xml"

                                sh 'python build.py --platform ios --build-system Xcode --config Release --module testbodenui --run-output-file testresults/ios_testbodenui.xml -- run --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
            }
        }
    }
}
