pipeline {
  agent any
  stages {
    stage('build') {
      steps {
        echo 'Building backend'
        sh 'chmod +x scripts/build.sh'
        sh 'scripts/build.sh'
      }
    }

    stage('test') {
      steps {
        echo 'Testing backend'
        sh 'chmod +x scripts/test.sh'
        sh 'scripts/test.sh'
      }
    }

    stage('deploy') {
      steps {
        echo 'Deploy backend'
        sh 'chmod +x scripts/deploy.sh'
        sh 'scripts/deploy.sh'
      }
    }
  }
	post {
    success {
			archiveArtifacts artifacts: 'backend',
			allowEmptyArchive: true,
			fingerprint: true,
			onlyIfSuccessful: true
		}
	}
}