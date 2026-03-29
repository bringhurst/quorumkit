import React, {useEffect} from 'react';

export default function Home() {
  useEffect(() => {
    window.location.replace('/docs/' + window.location.search + window.location.hash);
  }, []);

  return (
    <main style={{padding: '2rem', fontFamily: 'serif'}}>
      <p>
        Redirecting to <a href="/docs/">/docs/</a>...
      </p>
    </main>
  );
}
